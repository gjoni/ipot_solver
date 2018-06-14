/*
 * common.c
 *
 *  Created on: Jun 14, 2018
 *      Author: aivan
 */

#include <stdio.h>
#include <stdlib.h>

#include "common.h"

void check_malloc(void *ptr, char *msg) {

	if (ptr == NULL) {
		fprintf(stderr, "%s\n", msg);
		exit(1);
	}

}

