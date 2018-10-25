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
	double *z;
	double *e;
	int iter0;
	int iter;
	int niter;
	char *chk;

};

void problem_create(struct problem *P, struct graph *G);
void problem_free(struct problem *P);

int checkpoint_read(struct problem *P);
void checkpoint_write(struct problem *P);

double f(struct problem *P, const double *x);
void fdf(struct problem *P, const double *x, double *f, double *g);

lbfgsfloatval_t _evaluate(void *instance, const lbfgsfloatval_t *x,
		lbfgsfloatval_t *g, const int n, const lbfgsfloatval_t step);

int _progress(void *instance, const lbfgsfloatval_t *x,
		const lbfgsfloatval_t *g, const lbfgsfloatval_t fx,
		const lbfgsfloatval_t xnorm, const lbfgsfloatval_t gnorm,
		const lbfgsfloatval_t step, int n, int k, int ls);

int _update_problem(struct problem *P, const lbfgsfloatval_t *x, int iter);

void minimize(struct problem *P, int niter, char *chk);

#endif /* PROBLEM_H_ */
