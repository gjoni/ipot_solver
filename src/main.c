/*
 * main.c
 *
 *  Created on: Jun 4, 2018
 *      Author: aivan
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "args.h"
#include "graph.h"
#include "problem.h"

int main(int argc, char *argv[]) {

	/* (0) read command line arguments */
	struct args myargs = { NULL, NULL, NULL, 1.0, 100, 1 };
	if (!get_args(argc, argv, &myargs)) {
		print_args(&myargs);
		return 1;
	}

#if defined(_OPENMP)
	omp_set_num_threads(myargs.nthreads);
	printf("# %20s : %d\n", "num of threads", omp_get_max_threads());
#endif

	/* (1) read input graph */
	struct graph G;
	graph_read(myargs.in, &G);

	/* (2) set up the problem */
	struct problem P;
	problem_create(&P, &G);
	P.T = myargs.temp;
	P.iter = 0;
	printf("# %20s : %d/%d\n", "start/stop iteration", P.iter, myargs.niter);
	printf("# %20s : %.2f\n", "temperature", P.T);

	/* (3) read checkpoint, if any */
	if (myargs.chk != NULL) {
		checkpoint_read(&P, myargs.chk);
	}

	/* (4) minimize */
	minimize(&P, myargs.niter, myargs.chk);

	/* (5) zero-sum gauge */
	double avg = 0.0;
	double *J = P.J;
	for (int i = 0; i < P.G->dim; i++) {
		for (int j = 0; j < P.G->dim; j++) {
			avg += *(J++);
		}
	}
	avg /= (P.G->dim * P.G->dim);
	J = P.J;
	for (int i = 0; i < P.G->dim; i++) {
		printf("T%d %13.5e", i, P.h[i]);
		for (int j = 0; j < P.G->dim; j++) {
			printf(" %13.5e", *(J++) - avg);
		}
		printf("\n");
	}

	/* (9) free */
	problem_free(&P);
	graph_free(&G);

	return 0;

}
