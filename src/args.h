/*
 * args.h
 *
 *  Created on: Jun 4, 2018
 *      Author: aivan
 */

#ifndef ARGS_H_
#define ARGS_H_

struct args {

	char *in; /* input file */
	char *chk; /* checkpoint file */
	char *out; /* output file */
	double temp; /* learning temperature */
	int nthreads; /* number of threads */

};

void get_args(int argc, char *argv[], struct args *a);
void print_args(struct args *a);
//void print_cap();

#endif /* ARGS_H_ */
