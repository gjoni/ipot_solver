/*
 * problem.c
 *
 *  Created on: Jun 4, 2018
 *      Author: aivan
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "problem.h"
#include "common.h"

double E(unsigned char *neigh, unsigned char dim, const double *x, double *e) {

	double z = 0.0;

	const double *xptr = x + dim;

	/* probe every possible type */
	for (unsigned char j = 0; j < dim; j++) {

		/* self-energy */
		e[j] = x[j];

		/* pair interactions with neighbors */
		for (unsigned char k = 0; k < dim; k++) {
			e[j] += *(xptr++) * neigh[k];
		}

		e[j] = exp(-e[j]);
		z += e[j];

	}

	return 1.0 / z;

}

double f(struct problem *P, const double *x) {

	double lp = 0.0;

	struct graph *G = P->G;
	unsigned char DIM = G->dim;

#if defined(_OPENMP)
#pragma omp parallel for reduction(+:lp)
#endif
	for (long i = 0; i < G->nnodes; i++) {

		double z, e[DIM];

		z = E(G->neigh[i], DIM, x, e);
		lp += -log(e[G->type[i]] * z);

	}

	return lp;

}

void fdf(struct problem *P, const double *x, double *f, double *g) {

	double lp = 0.0;

	struct graph *G = P->G;
	unsigned char DIM = G->dim;
	int SIZE = DIM + DIM * DIM;

	memset(g, 0, SIZE * sizeof(double));

#if defined(_OPENMP)
#pragma omp parallel for reduction(+:lp,g[:SIZE])
#endif
	for (long i = 0; i < G->nnodes; i++) {


		/* pre-compute local energies for every node */
		double *e = P->e + i * DIM;
		P->z[i] = E(G->neigh[i], DIM, x, e);
		lp += -log(e[G->type[i]] * P->z[i]);

		for (unsigned char a = 0; a < DIM; a++) {
			e[a] = (-e[a] * P->z[i] + 1.0 * (G->type[i] == a));
		}

		/* derivatives */
		for (int a = 0; a < DIM; a++) {

			/* local fields */
			g[a] += e[a];

			/* couplings */
			for (unsigned char b = 0; b < DIM; b++) {
				g[DIM * (a + 1) + b] += e[a] * G->neigh[i][b];
			}
		}

	}

	/* symmetrize */
	for (unsigned char a = 0; a < DIM; a++) {
		for (unsigned char b = a + 1; b < DIM; b++) {
			double *ga = g + DIM + a * DIM + b;
			double *gb = g + DIM + b * DIM + a;
			*ga += *gb;
			*gb = *ga;
		}

	}

	*f = lp;

}

void problem_create(struct problem *P, struct graph *G) {

	unsigned char dim = G->dim;

	P->G = G;

	P->h = (double*) calloc(dim, sizeof(double));
	check_malloc(P->h, "cannot allocate P->h");

	P->J = (double*) calloc(dim * dim, sizeof(double));
	check_malloc(P->J, "cannot allocate P->J");

	P->z = (double*) malloc(P->G->nnodes * sizeof(double));
	check_malloc(P->J, "cannot allocate P->z");

	P->e = (double*) malloc(P->G->nnodes * dim * sizeof(double));
	check_malloc(P->e, "cannot allocate P->e");
	
	P->iter0 = 0;
	P->iter = 0;
	P->niter = 0;

	P->chk = NULL;

}

void problem_free(struct problem *P) {

	P->G = NULL;

	free(P->h);
	free(P->J);
	free(P->z);
	free(P->e);

}

void checkpoint_write(struct problem *P) {
	
	FILE *F = fopen(P->chk, "w");
	if (F == NULL) {
		fprintf(stderr, "Error: cannot open '%s' file for writing\n", P->chk);
		exit(1);
	}

	fprintf(F, "# %d %d\n", P->G->dim, P->iter);

	double *J = P->J;
	for (int i = 0; i < P->G->dim; i++) {
		fprintf(F, "%.10lf", P->h[i]);
		for (int j = 0; j < P->G->dim; j++) {
			fprintf(F, " %.10lf", *(J++));
		}
		fprintf(F, "\n");
	}

	fclose(F);

}

int checkpoint_read(struct problem *P) {

	FILE *F = fopen(P->chk, "r");
	if (F == NULL) {
		return 1;
	}

	int dim;
	if (fscanf(F, "# %d %d\n", &dim, &(P->iter0)) != 2) {
		fprintf(stderr, "Error: misformatted checkpoint file '%s'\n", P->chk);
		exit(1);
	}

	if (dim != P->G->dim) {
		fprintf(stderr,
				"Error: dimension in the checkpoint file '%s' do not match dimension in the Graph (%d != %d)\n",
				P->chk, dim, P->G->dim);
		exit(1);
	}

	double *J = P->J;
	for (int i = 0; i < dim; i++) {
		fscanf(F, "%lf", P->h + i);
		for (int j = 0; j < dim; j++) {
			fscanf(F, "%lf", J++);
		}
	}

	fclose(F);

	return 0;

}

int _update_problem(struct problem *P, const lbfgsfloatval_t *x, int iter) {

	P->iter = iter + P->iter0;
	double *J = P->J;
	for (int i = 0; i < P->G->dim; i++) {
		P->h[i] = x[i];
		for (int j = 0; j < P->G->dim; j++)  {
			*(J++) = x[P->G->dim + i * P->G->dim + j];
		}
	}

	return iter;

}

void minimize(struct problem *P, int niter, char *chk) {

	struct graph *G = P->G;
	unsigned char dim = G->dim;

	double *x = (double*) calloc((dim * (dim + 1)), sizeof(double));

	printf("# %-8s%-14s%-14s%-14s%-8s%-12s\n", "iter", "f(x)", "||x||", "||g||",
			"neval", "epsilon");
	//printf("# %-8d%-12.5e\n", 0, f(P, x));

	assert(sizeof(double) == sizeof(lbfgsfloatval_t)); /* sse2 disabled */

	int size = dim * dim + dim;

	lbfgsfloatval_t fx;
	lbfgsfloatval_t *m_x = lbfgs_malloc(size);
	check_malloc(m_x, "cannot allocate m_x");

	/* init x[] with P->h[] and P->J[] */
	{
		double *J = P->J;
		for (int i = 0; i < P->G->dim; i++) {
			m_x[i] = P->h[i];
			for (int j = 0; j < P->G->dim; j++)  {
				m_x[P->G->dim + i * P->G->dim + j] = *(J++);
			}
		}
	}

	lbfgs_parameter_t param;
	lbfgs_parameter_init(&param);
	param.max_iterations = niter;

	lbfgs(size, m_x, &fx, _evaluate, _progress, P, &param);

	free(x);
	free(m_x);

}

int _progress(void *instance, const lbfgsfloatval_t *x,
		const lbfgsfloatval_t *g, const lbfgsfloatval_t fx,
		const lbfgsfloatval_t xnorm, const lbfgsfloatval_t gnorm,
		const lbfgsfloatval_t step, int n, int k, int ls) {

	struct problem *P = (struct problem*) instance;

	printf("# %-8d%-12.5e  %-12.5e  %-12.5e  %-6d  %-10.5f\n", 
			k + P->iter0, fx, xnorm, gnorm, ls, gnorm / xnorm);
	fflush(stdout);

	_update_problem(P, x, k);

	if (P->chk != NULL) {
		checkpoint_write(P);
	}
	
	if (P->iter >= P->niter) {
		return 1;
	}
	
	return 0;

}

lbfgsfloatval_t _evaluate(void *instance, const lbfgsfloatval_t *x,
		lbfgsfloatval_t *g, const int n, const lbfgsfloatval_t step) {

	struct problem *P = (struct problem*) instance;
	lbfgsfloatval_t f;
	fdf(P, x, &f, g);
	
	return f;

}

