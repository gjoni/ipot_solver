/*
 * graph.c
 *
 *  Created on: Jun 6, 2018
 *      Author: aivan
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "common.h"

void graph_create(struct graph *g, long nnodes, unsigned char dim) {

	g->nnodes = nnodes;
	g->dim = dim;

	g->type = (unsigned char*) malloc(g->nnodes * sizeof(unsigned char));
	check_malloc(g->type, "cannot allocate g->type");

	g->neigh = (unsigned char**) malloc(g->nnodes * sizeof(unsigned char*));
	check_malloc(g->neigh, "cannot allocate g->neigh");

	for (long i = 0; i < g->nnodes; i++) {
		g->neigh[i] = (unsigned char*) malloc(g->dim * sizeof(unsigned char));
		check_malloc(g->neigh[i], "cannot allocate g->neigh");
	}

}

void graph_free(struct graph *g) {

	free(g->type);
	for (long i = 0; i < g->nnodes; i++) {
		free(g->neigh[i]);
	}

}

void graph_read(char *name, struct graph *g) {

	FILE *F = fopen(name, "r");
	if (F == NULL) {
		fprintf(stderr, "Error: cannot read from '%s' input file\n", name);
		exit(1);
	}

	const int SIZE = 8192;
	char buf[SIZE];

	/* skip caption */
	int ncap = 0;
	while (fgets(buf, SIZE, F) && buf[0] == '#') {
		ncap++;
	}

	/* get dimension from the first line */
	int dim = -1;
	char * pch = strtok(buf, " \t\n");
	while (pch != NULL) {
		pch = strtok(NULL, " \t\n");
		dim++;
	}

	/* check dimensions */
	if (dim > 256 || dim < 2) {
		fprintf(stderr, "Error: graph dimension should be in [2;256] range\n");
		fclose(F);
		exit(1);
	}

	long nnodes = 1;
	while (fgets(buf, SIZE, F)) {
		if (buf[0] != '\n' && buf[0] != ' ') {
			nnodes++;
		}
	}

	/* allocate memory for the graph */
	graph_create(g, nnodes, dim);

	/* read graph structure */
	rewind(F);

	for (int i = 0; i < ncap; i++) {
		fgets(buf, SIZE, F);
	}

	for (long i = 0; i < nnodes; i++) {
		fgets(buf, SIZE, F);
		pch = strtok(buf, " \t\n");
		g->type[i] = atoi(pch);
		for (int j = 0; j < dim; j++) {
			pch = strtok(NULL, " \t\n");
			g->neigh[i][j] = atoi(pch);
		}
	}

	printf("# %20s : %d x %ld\n", "ntypes x nnodes", dim, nnodes);
	printf("# %20s : %.1fmb\n", "graph size",
			1.0 * (dim + 1) * nnodes / 1024.0 / 1024.0);
	printf("# %20s : %.1fmb\n", "temp variables size",
			8.0 * (dim + 1) * nnodes / 1024.0 / 1024.0);

	fclose(F);

}

