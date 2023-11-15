#include "kmeans_char.h"

inline static int find_nearest_cluster(int     numClusters, /* no. clusters */
                                       int     numCoords,   /* no. coordinates */
                                       unsigned char * object,      /* [numCoords] */
                                       unsigned char * clusters,    /* [numClusters][numCoords] */
                                       int type)            /* 0: euclidean distance, 1: cosine similarity, 2: hamming distance */
{
    int index = 0, i;
    if(type == 0){ // hamming distance
        long dist, min_dist;

        // find the cluster id that has min distance to object 
        min_dist = bits_hamming_distance(object, clusters, numCoords);

        for(i=1; i<numClusters; i++) {
            dist = bits_hamming_distance(object, &clusters[i*numCoords], numCoords);
            // no need square root 
            if (dist < min_dist) { // find the min and its array index
                min_dist  = dist;
                index     = i;
            }
        }
    }
    return index;
}


void kmeans_char(unsigned char * objects,          /* in: [numObjs][numCoords] */
                 int             numCoords,        /* no. coordinates */
                 int             numObjs,          /* no. objects */
                 int             numClusters,      /* no. clusters */
                 float           threshold,        /* minimum fraction of objects that change membership */
                 long            loop_threshold,   /* maximum number of iterations */
                 int           * membership,       /* out: [numObjs] */
                 unsigned char * clusters,         /* out: [numClusters][numCoords] */
                 int             type)             /* 0: euclidean distance, 1: cosine similarity, 2: hamming distance */
{
    int i, j, k;
    int index, loop=0;

    float delta;          // fraction of objects whose clusters change in each loop 
    int * newClusterSize; // [numClusters]: no. objects assigned in each new cluster 
    float * newClusters;  // [numClusters][numCoords] 
    int nthreads;         // no. threads 

    nthreads = omp_get_max_threads();
    printf("OpenMP Kmeans - Reduction\t(number of threads: %d)\n", nthreads);

    // initialize membership
    for (i=0; i<numObjs; i++)
        membership[i] = -1;

    // initialize newClusterSize and newClusters to all 0 
    newClusterSize = (typeof(newClusterSize)) calloc(numClusters, sizeof(*newClusterSize));
    newClusters = (typeof(newClusters))  calloc(numClusters * numCoords, sizeof(*newClusters));

    // Each thread calculates new centers using a private space. After that, thread 0 does an array reduction on them.
    int * local_newClusterSize[nthreads];  // [nthreads][numClusters] 
    float * local_newClusters[nthreads];   // [nthreads][numClusters][numCoords]

    /*
     * Hint for false-sharing
     * This is noticed when numCoords is low (and neighboring local_newClusters exist close to each other).
     * Allocate local cluster data with a "first-touch" policy.
     */
    // Initialize local (per-thread) arrays (and later collect result on global arrays)
    #pragma omp parallel for
    for (k=0; k<nthreads; k++)
    {
        local_newClusterSize[k] = (typeof(*local_newClusterSize)) calloc(numClusters, sizeof(**local_newClusterSize));
        local_newClusters[k] = (typeof(*local_newClusters)) calloc(numClusters * numCoords, sizeof(**local_newClusters));
    }

    // timing = wtime();
    do {
        // before each loop, set cluster data to 0
        for (i=0; i<numClusters; i++) {
            for (j=0; j<numCoords; j++)
                newClusters[i*numCoords + j] = 0.0;
            newClusterSize[i] = 0;
        }

        delta = 0.0;

        #pragma omp parallel \
        private(i,j,index) \
        firstprivate(numObjs,numClusters,numCoords) \
        shared(objects,clusters,membership,newClusters,newClusterSize)
        {
            /* 
             * TODO: Initiliaze local cluster data to zero (separate for each thread)
             */
            int tid = omp_get_thread_num();
            for (i=0; i<numClusters; i++){
                for (j=0; j<numCoords; j++)
                    local_newClusters[tid][i*numCoords + j] = 0;
                local_newClusterSize[tid][i] = 0;
            }

            #pragma omp for schedule(static) reduction(+:delta)
            for (i=0; i<numObjs; i++)
            {
                // find the array index of nearest cluster center 
                index = find_nearest_cluster(numClusters, numCoords, &objects[i*numCoords], clusters, type); // object is a bitstream
                
                // if membership changes, increase delta by 1 
                if (membership[i] != index){
                    delta += 1.0;
                    // if(loop>0)
                    //     printf("loop %d, membership %d = %d ( it was %d)\n", loop, i, membership[i], index);

                }
                
                // assign the membership to object i 
                membership[i] = index;

                // update new cluster centers : sum of all objects located within (average will be performed later) 
                /* 
                 * TODO: Collect cluster data in local arrays (local to each thread)
                 *       Replace global arrays with local per-thread
                 */
                tid = omp_get_thread_num();
                local_newClusterSize[tid][index]++;
                for (j=0; j<numCoords; j++){
                    local_newClusters[tid][index*numCoords + j] += objects[i*numCoords + j]; // ti timi exei to bit
                }
            }

            /*
             * TODO: Reduction of cluster data from local arrays to shared.
             *       This operation will be performed by one thread
             */
            #pragma omp single
            {
                for (k=0; k<nthreads; k++)
                {
                    for (i=0; i<numClusters; i++){
                        newClusterSize[i] += local_newClusterSize[k][i];
                        for (j=0; j<numCoords; j++){
                            newClusters[i*numCoords + j] += local_newClusters[k][i*numCoords + j];
                        }
                    }
                }
            }
        }

        // average the sum and replace old cluster centers with newClusters 
        for (i=0; i<numClusters; i++) {
            if (newClusterSize[i] > 0) {
                // if(i==0) printf("loop = %d cluster%d = [", loop, i);
                for (j=0; j<numCoords; j++) {
                    clusters[i*numCoords + j] = (newClusters[i*numCoords + j] / newClusterSize[i]+0.5);
                    // if(i==0) printf("%d", clusters[i*numCoords + j]);
                }
                // if (i==0) printf("]\n");
            }
        }

        printf("\r\tcompleted loop %d, delta = %lf\n", loop, delta);
        fflush(stdout);
        // Get fraction of objects whose membership changed during this loop. This is used as a convergence criterion.
        delta /= numObjs;
        
        loop++;
    } while (delta > threshold && loop < loop_threshold);

    for (k=0; k<nthreads; k++)
    {
        free(local_newClusterSize[k]);
        free(local_newClusters[k]);
    }
    free(newClusters);
    free(newClusterSize);
}
