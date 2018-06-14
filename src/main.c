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
	printf("# nthreads : %d\n", omp_get_max_threads());
#endif

	/* (1) read input graph */
	struct graph G;
	graph_read(myargs.in, &G);

	/* (2) set up the problem */
	struct problem P;
	problem_create(&P, &G);
	P.T = myargs.temp;
	P.iter = 0;

	/* (3) read checkpoint, if any */
	if (myargs.chk != NULL) {

	}

	/* (4) minimize */
	minimize(&P, myargs.niter);

	/* (9) free */
	problem_free(&P);
	graph_free(&G);

	return 0;

}
