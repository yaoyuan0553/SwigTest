//
// Created by yuan on 8/28/19.
//

#pragma once
#ifndef SWIGTEST_UTILITY_H
#define SWIGTEST_UTILITY_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>


/* macro for printing errors and exit program */
#define PERROR(x...)                                                    \
    do {                                                                \
        fprintf(stderr, "[-] ERROR: " x);                               \
		fprintf(stderr, "\n\tLocation : %s(), %s:%u\n", __FUNCTION__,   \
			__FILE__, __LINE__);                                        \
        exit(-1);                                                       \
    } while (0)

/* macro for printing system errors stored in errno and exit program */
#define PSYS_FATAL(x...)                                                \
	do {                                                                \
		fprintf(stderr, "[-] SYSTEM ERROR : " x);                       \
		fprintf(stderr, "\n\tLocation : %s(), %s:%u\n", __FUNCTION__,   \
			__FILE__, __LINE__);                                        \
		perror("      OS message ");                                    \
		fprintf(stderr, "%d: %s\n", errno, strerror(errno));            \
		exit(EXIT_FAILURE);                                             \
	} while (0)


#endif //SWIGTEST_UTILITY_H
