/*
 * args.c
 *
 *  Created on: Jun 4, 2018
 *      Author: aivan
 */

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "args.h"

bool get_args(int argc, char *argv[], struct args *myargs) {

	char tmp;
	while ((tmp = getopt(argc, argv, "hi:o:c:t:T:n:")) != -1) {
		switch (tmp) {
		case 'h': /* help */
			printf("!!! HELP !!!\n");
			return false;
			break;
		case 'i': /* input file */
			myargs->in = optarg;
			break;
		case 'o': /* output file */
			myargs->out = optarg;
			break;
		case 'c': /* checkpoint file */
			myargs->chk = optarg;
			break;
		case 't': /* number of threads */
			myargs->nthreads = atoi(optarg);
			break;
		case 'T': /* learning temperature */
			myargs->temp = atof(optarg);
			break;
		case 'n': /* number of iterations */
			myargs->niter = atoi(optarg);
			break;
		default:
			return false;
			break;
		}
	}

	if (myargs->in == NULL) {
		printf("Error: input file not specified ('-i')\n");
		return false;
	}

	return true;

}

void print_args(struct args *myargs) {

	printf("\nUsage:   ./ipot_solver [-option] [argument]\n\n");
	printf("Options:  -i input.txt                   - input, required\n");
	printf("          -o output.txt                  - output, optional\n");
	printf("          -c checkpoint.txt              - input/output, optional\n");
	printf("          -T learning temperature          %.2f\n", myargs->temp);
	printf("          -n number of iterations          %d\n", myargs->niter);
	printf("          -t number of threads             %d\n", myargs->nthreads);
	printf("\n");

}
