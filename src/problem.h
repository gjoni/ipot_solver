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

struct problem {
	struct graph *G;
	double *h;
	double *J;
	double T;
	int iter;
};

void problem_create(struct problem *P, struct graph *G);
void problem_free(struct problem *P);

void problem_read(struct problem *P, char *name);
void problem_write(struct problem *P, char *name);

double f(struct graph *G, const double *x);
void fdf(struct graph *G, const double *x, double *f, double *g);

lbfgsfloatval_t _evaluate(void *instance, const lbfgsfloatval_t *x,
		lbfgsfloatval_t *g, const int n, const lbfgsfloatval_t step);

int _progress(void *instance, const lbfgsfloatval_t *x,
		const lbfgsfloatval_t *g, const lbfgsfloatval_t fx,
		const lbfgsfloatval_t xnorm, const lbfgsfloatval_t gnorm,
		const lbfgsfloatval_t step, int n, int k, int ls);

void minimize(struct problem *P, int niter);

#endif /* PROBLEM_H_ */
