/*
 * graph.h
 *
 *  Created on: Jun 6, 2018
 *      Author: aivan
 */

#ifndef GRAPH_H_
#define GRAPH_H_

struct graph {
	long nnodes;
	unsigned char dim;
	unsigned char *type;
	unsigned char **neigh;
};

void graph_create(struct graph *g, long nnodes, unsigned char dim);
void graph_free(struct graph *g);
void graph_read(char *name, struct graph *g);

#endif /* GRAPH_H_ */
