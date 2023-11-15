#ifndef KMEANS_CHAR_H
#define KMEANS_CHAR_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "bit_ops.h"

void kmeans_char(unsigned char * objects, int numCoords, int numObjs, int numClusters, float threshold, long loop_threshold, int * membership, unsigned char * clusters, int type);

#endif /* KMEANS_CHAR_H */
