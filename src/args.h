/*
 * args.h
 *
 *  Created on: Jun 4, 2018
 *      Author: aivan
 */

#ifndef ARGS_H_
#define ARGS_H_

#include <stdbool.h>

struct args {

	char *in; /* input file */
	char *chk; /* checkpoint file */
	char *out; /* output file */
	double temp; /* learning temperature */
	int nthreads; /* number of threads */

};

bool get_args(int argc, char *argv[], struct args *a);

void print_args(struct args *a);

#endif /* ARGS_H_ */
