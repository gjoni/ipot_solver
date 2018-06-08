/*
 * problem.c
 *
 *  Created on: Jun 4, 2018
 *      Author: aivan
 */

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

