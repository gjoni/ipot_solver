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

#include "problem.h"

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

double f(struct graph *G, const double *x) {

	double lp = 0.0;

	unsigned char DIM = G->dim;

#if defined(_OPENMP)
#pragma omp parallel for schedule(dynamic) reduction(+:lp)
#endif
	for (long i = 0; i < G->nnodes; i++) {

		double z, e[DIM];

		z = E(G->neigh[i], DIM, x, e);
		lp += -log(e[G->type[i]] * z);

	}

	return lp;

}

void fdf(struct graph *G, const double *x, double *f, double *g) {

	double lp = 0.0;

	unsigned char DIM = G->dim;

	memset(g, 0, (DIM + 1) * DIM * sizeof(double));

	for (long i = 0; i < G->nnodes; i++) {

		double z, e[DIM];

		z = E(G->neigh[i], DIM, x, e);
		lp += -log(e[G->type[i]] * z);

		for (unsigned char a = 0; a < DIM; a++) {
			e[a] = -e[a] * z + 1.0 * (G->type[i] == a);
		}

		/* local fields */
		for (unsigned char a = 0; a < DIM; a++) {
			g[a] += e[a];
		}

		/* couplings */
		double *gptr = g + DIM;
		for (unsigned char a = 0; a < DIM; a++) {
			for (unsigned char b = 0; b < DIM; b++) {
				*(gptr++) += e[a] * G->neigh[i][b];
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
	P->J = (double*) calloc(dim * dim, sizeof(double));

	P->T = 1.0;

}

void problem_free(struct problem *P) {

	P->G = NULL;

	free(P->h);
	free(P->J);

}

void problem_read(struct problem *P, char *name) {

	FILE *F = fopen(name, "r");
	if (F == NULL) {
		fprintf(stderr, "Error: cannot open '%s' file for reading\n", name);
		exit(1);
	}

	int dim;
	if (fscanf(F, "# %d %lf %d\n", &dim, &(P->T), &(P->iter)) != 3) {
		fprintf(stderr, "Error: misformatted checkpoint file '%s'\n", name);
		exit(1);
	}

	if (dim != P->G->dim) {
		fprintf(stderr,
				"Error: mismatched dimension in the checkpoint file '%s' (%d != %d)\n",
				name, dim, P->G->dim);
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

}

void minimize(struct problem *P, int niter) {

	struct graph *G = P->G;
	unsigned char dim = G->dim;

	double *x = (double*) calloc((dim * (dim + 1)), sizeof(double));

	printf("# %-8s%-14s%-14s%-14s%-8s%-12s\n", "iter", "f(x)", "||x||", "||g||",
			"neval", "epsilon");
	printf("# %-8d%-12.5e\n", 0, f(G, x));

	free(x);

}

int _progress(void *instance, const lbfgsfloatval_t *x,
		const lbfgsfloatval_t *g, const lbfgsfloatval_t fx,
		const lbfgsfloatval_t xnorm, const lbfgsfloatval_t gnorm,
		const lbfgsfloatval_t step, int n, int k, int ls) {

	printf("# %-8d%-12.5e  %-12.5e  %-12.5e  %-6d  %-10.5f\n", k, fx, xnorm,
			gnorm, ls, gnorm / xnorm);
	fflush(stdout);

	return 0;

}
