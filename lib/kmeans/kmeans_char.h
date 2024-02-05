#ifndef KMEANS_CHAR_H
#define KMEANS_CHAR_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "bit_ops.h"

void kmeans_char(unsigned char * objects, int numCoords, int numObjs, int numClusters, float threshold, int loop_threshold, int * membership, unsigned char * clusters, int type);

void kmeans_char2_csr(int * rc_r, int * rc_c, unsigned char * rc_v, int numCoords, int numObjs, int numClusters, float threshold, int loop_threshold, int * membership, unsigned char * clusters, int type);
void kmeans_char2_csc(int * cc_r, int * cc_c, unsigned char * cc_v, int numCoords, int numObjs, int numClusters, float threshold, int loop_threshold, int * membership, unsigned char * clusters, int type);

#endif /* KMEANS_CHAR_H */
