/*
 * problem.h
 *
 *  Created on: Jun 4, 2018
 *      Author: aivan
 */

#ifndef PROBLEM_H_
#define PROBLEM_H_

#include "graph.h"
#include "lbfgs.h"


/* TODO:
 * + 1) include temp variables and allocate memory only once
 *   2) change *J to **J
 *   3) ??? smth about objective function ??? */

struct problem {
	struct graph *G;
	double *h;
	double *J;
//	double **J;
	double *z;
	double *e;
	double T;
	int iter;
};

void problem_create(struct problem *P, struct graph *G);
void problem_free(struct problem *P);

void checkpoint_read(struct problem *P, char *name);
void checkpoint_write(struct problem *P, char *name);

double f(struct problem *P, const double *x);
void fdf(struct problem *P, const double *x, double *f, double *g);

lbfgsfloatval_t _evaluate(void *instance, const lbfgsfloatval_t *x,
		lbfgsfloatval_t *g, const int n, const lbfgsfloatval_t step);

int _progress(void *instance, const lbfgsfloatval_t *x,
		const lbfgsfloatval_t *g, const lbfgsfloatval_t fx,
		const lbfgsfloatval_t xnorm, const lbfgsfloatval_t gnorm,
		const lbfgsfloatval_t step, int n, int k, int ls);

void minimize(struct problem *P, int niter, char *chk);

#endif /* PROBLEM_H_ */
