/*
 * main.c
 *
 *  Created on: Jun 4, 2018
 *      Author: aivan
 */

#include <stdio.h>
#include <stdlib.h>

#include "args.h"
#include "graph.h"
#include "problem.h"

int main(int argc, char *argv[]) {

	/* (0) read command line arguments */
	struct args myargs = { NULL, NULL, NULL, 1.0, 1 };
	if (!get_args(argc, argv, &myargs)) {
		print_args(&myargs);
		return 1;
	}

	/* (1) read input graph */
	struct graph g;
	graph_read(myargs.in, &g);

	/* (9) free */
	graph_free(&g);

	return 0;

}
