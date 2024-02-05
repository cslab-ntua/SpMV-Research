#ifndef KMEANS_H
#define KMEANS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

void kmeans(float * objects, int numCoords, int numObjs, int numClusters, float threshold, int loop_threshold, int * membership, float * clusters, int type);

void kmeans2_csr(int * rc_r, int * rc_c, float * rc_v, int numCoords, int numObjs, int numClusters, float threshold, int loop_threshold, int * membership, float * clusters, int type);
void kmeans2_csc(int * cc_r, int * cc_c, float * cc_v, int numCoords, int numObjs, int numClusters, float threshold, int loop_threshold, int * membership, float * clusters, int type);

#endif /* KMEANS_H */
