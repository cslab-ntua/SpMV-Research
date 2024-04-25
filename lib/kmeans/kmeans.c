#include "kmeans.h"

// square of Euclid distance between two multi-dimensional points
inline static float euclid_dist_2(int    numdims,  /* no. dimensions */
								 float * coord1,   /* [numdims] */
								 float * coord2)   /* [numdims] */
{
	float ans = 0.0;
	for(int i=0; i<numdims; i++)
		ans += (coord1[i]-coord2[i]) * (coord1[i]-coord2[i]);
	return ans;
}

// square of Euclid distance between two multi-dimensional points
inline static float cosine_similarity_2(int    numdims,  /* no. dimensions */
										float * coord1,   /* [numdims] */
										float * coord2)   /* [numdims] */
{
	float ans = 0.0;
	float dotprod = 0.0;
	float mag1 = 0.0, mag2 = 0.0;

	for(int i=0; i<numdims; i++){
		dotprod += coord1[i]*coord2[i];
		mag1 += coord1[i]*coord1[i];
		mag2 += coord2[i]*coord2[i];
	}
	mag1 = sqrt(mag1);
	mag2 = sqrt(mag2);

	if(mag1 == 0.0 || mag2 == 0.0)
		ans = 0.0;
	else
		ans = dotprod / (mag1 * mag2);

	return ans;
}

inline static int find_nearest_cluster(int     numClusters, /* no. clusters */
									   int     numCoords,   /* no. coordinates */
									   float * object,      /* [numCoords] */
									   float * clusters,    /* [numClusters][numCoords] */
									   int type)            /* 0: euclidean distance, 1: cosine similarity, 2: hamming distance */
{
	int index = 0;
	if(type == 0){ // cosine similarity
		float simil, max_simil;

		// find the cluster id that has min distance to object 
		max_simil = cosine_similarity_2(numCoords, object, clusters);

		for(int i=1; i<numClusters; i++) {
			simil = cosine_similarity_2(numCoords, object, &clusters[i*numCoords]);
			// no need square root 
			if (simil > max_simil) { // find the min and its array index
				max_simil = simil;
				index     = i;
			}
		}
	}
	else if(type == 1){ // euclidean distance
		float dist, min_dist;

		// find the cluster id that has min distance to object 
		min_dist = euclid_dist_2(numCoords, object, clusters);

		for(int i=1; i<numClusters; i++) {
			dist = euclid_dist_2(numCoords, object, &clusters[i*numCoords]);
			// no need square root 
			if (dist < min_dist) { // find the min and its array index
				min_dist  = dist;
				index     = i;
			}
		}
	}
	return index;
}


void kmeans(float * objects,          /* in: [numObjs][numCoords] */
			int     numCoords,        /* no. coordinates */
			int     numObjs,          /* no. objects */
			int     numClusters,      /* no. clusters */
			float   threshold,        /* minimum fraction of objects that change membership */
			int     loop_threshold,   /* maximum number of iterations */
			int   * membership,       /* out: [numObjs] */
			float * clusters,         /* out: [numClusters][numCoords] */
			int type)                 /* 0: euclidean distance, 1: cosine similarity, 2: hamming distance */
{
	int i, j, k;
	int index, loop=0;

	float delta;          // fraction of objects whose clusters change in each loop 
	int * newClusterSize; // [numClusters]: no. objects assigned in each new cluster 
	float * newClusters;  // [numClusters][numCoords] 
	int nthreads;         // no. threads 

	nthreads = omp_get_max_threads();
	// printf("OpenMP Kmeans - Reduction\t(number of threads: %d)\n", nthreads);

	// initialize membership
	for (i=0; i<numObjs; i++)
		membership[i] = -1;

	// initialize newClusterSize and newClusters to all 0 
	newClusterSize = (typeof(newClusterSize)) calloc(numClusters, sizeof(*newClusterSize));
	newClusters = (typeof(newClusters))  calloc(numClusters * numCoords, sizeof(*newClusters));

	// Each thread calculates new centers using a private space. After that, thread 0 does an array reduction on them.
	int * local_newClusterSize[nthreads];  // [nthreads][numClusters] 
	float * local_newClusters[nthreads];   // [nthreads][numClusters][numCoords]

	// Initialize local (per-thread) arrays (and later collect result on global arrays)
	#pragma omp parallel for
	for (k=0; k<nthreads; k++)
	{
		local_newClusterSize[k] = (typeof(*local_newClusterSize)) calloc(numClusters, sizeof(**local_newClusterSize));
		local_newClusters[k] = (typeof(*local_newClusters)) calloc(numClusters * numCoords, sizeof(**local_newClusters));
	}

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
			int tid = omp_get_thread_num();
			for (i=0; i<numClusters; i++){
				for (j=0; j<numCoords; j++)
					local_newClusters[tid][i*numCoords + j] = 0.0;
				local_newClusterSize[tid][i] = 0;
			}

			#pragma omp for schedule(static) reduction(+:delta)
			for (i=0; i<numObjs; i++)
			{
				// find the array index of nearest cluster center 
				index = find_nearest_cluster(numClusters, numCoords, &objects[i*numCoords], clusters, type);
				
				// if membership changes, increase delta by 1 
				if (membership[i] != index)
					delta += 1.0;
				
				// assign the membership to object i 
				membership[i] = index;

				// update new cluster centers : sum of all objects located within (average will be performed later) 
				tid = omp_get_thread_num();
				local_newClusterSize[tid][index]++;
				for (j=0; j<numCoords; j++)
					local_newClusters[tid][index*numCoords + j] += objects[i*numCoords + j];
			}

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
				for (j=0; j<numCoords; j++) {
					clusters[i*numCoords + j] = newClusters[i*numCoords + j] / newClusterSize[i];
				}
			}
		}

		printf("\tloop = %d, delta = %lf\n", loop, delta);

		// Get fraction of objects whose membership changed during this loop. This is used as a convergence criterion.
		delta /= numObjs;
		
		loop++;
		printf("\r\tcompleted loop %d", loop);
		fflush(stdout);
	} while (delta > threshold && loop < loop_threshold);

	for (k=0; k<nthreads; k++)
	{
		free(local_newClusterSize[k]);
		free(local_newClusters[k]);
	}
	free(newClusters);
	free(newClusterSize);
}

void kmeans2_csr(int   * rc_r,             /* in: [numObjs] */
				 int   * rc_c,             /* in: [numVals] */
				 float * rc_v,             /* in: [numVals] */
				 int     numCoords,        /* no. coordinates */
				 int     numObjs,          /* no. objects */
				 int     numClusters,      /* no. clusters */
				 float   threshold,        /* minimum fraction of objects that change membership */
				 int     loop_threshold,   /* maximum number of iterations */
				 int   * membership,       /* out: [numObjs] */
				 float * clusters,         /* out: [numClusters][numCoords] */
				 int type)                 /* 0: euclidean distance, 1: cosine similarity, 2: hamming distance */
{
	int i, j, k;
	int index, loop=0;

	float delta;          // fraction of objects whose clusters change in each loop 
	int * newClusterSize; // [numClusters]: no. objects assigned in each new cluster 
	float * newClusters;  // [numClusters][numCoords] 
	int nthreads;         // no. threads 

	nthreads = omp_get_max_threads();
	// printf("OpenMP Kmeans - Reduction\t(number of threads: %d)\n", nthreads);

	// initialize membership
	for (i=0; i<numObjs; i++)
		membership[i] = -1;

	// initialize newClusterSize and newClusters to all 0 
	newClusterSize = (typeof(newClusterSize)) calloc(numClusters, sizeof(*newClusterSize));
	newClusters = (typeof(newClusters))  calloc(numClusters * numCoords, sizeof(*newClusters));

	// Each thread calculates new centers using a private space. After that, thread 0 does an array reduction on them.
	int * local_newClusterSize[nthreads];  // [nthreads][numClusters] 
	float * local_newClusters[nthreads];   // [nthreads][numClusters][numCoords]

	// Initialize local (per-thread) arrays (and later collect result on global arrays)
	#pragma omp parallel for
	for (k=0; k<nthreads; k++)
	{
		local_newClusterSize[k] = (typeof(*local_newClusterSize)) calloc(numClusters, sizeof(**local_newClusterSize));
		local_newClusters[k] = (typeof(*local_newClusters)) calloc(numClusters * numCoords, sizeof(**local_newClusters));
	}

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
		shared(rc_r,rc_c,rc_v,clusters,membership,newClusters,newClusterSize)
		{
			int tid = omp_get_thread_num();
			for (i=0; i<numClusters; i++){
				for (j=0; j<numCoords; j++)
					local_newClusters[tid][i*numCoords + j] = 0.0;
				local_newClusterSize[tid][i] = 0;
			}

			#pragma omp for schedule(static) reduction(+:delta)
			for (i=0; i<numObjs; i++)
			{
				float * local_object = (typeof(local_object)) calloc(numCoords, sizeof(*local_object));
				for(j=rc_r[i]; j<rc_r[i+1]; j++)
					local_object[rc_c[j]] = rc_v[j];
				// find the array index of nearest cluster center 
				// index = find_nearest_cluster(numClusters, numCoords, &objects[i*numCoords], clusters, type);
				index = find_nearest_cluster(numClusters, numCoords, local_object, clusters, type);
				
				// if membership changes, increase delta by 1 
				if (membership[i] != index)
					delta += 1.0;
				
				// assign the membership to object i 
				membership[i] = index;

				// update new cluster centers : sum of all objects located within (average will be performed later) 
				tid = omp_get_thread_num();
				local_newClusterSize[tid][index]++;
				for (j=0; j<numCoords; j++)
					// local_newClusters[tid][index*numCoords + j] += objects[i*numCoords + j];
					local_newClusters[tid][index*numCoords + j] += local_object[j];
				free(local_object);
			}

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
				for (j=0; j<numCoords; j++) {
					clusters[i*numCoords + j] = newClusters[i*numCoords + j] / newClusterSize[i];
				}
			}
		}

		printf("\tloop = %d, delta = %lf\n", loop, delta);

		// Get fraction of objects whose membership changed during this loop. This is used as a convergence criterion.
		delta /= numObjs;
		
		int * stats = (typeof(stats)) calloc(numClusters, sizeof(*stats));
		for(int i=0;i<numObjs;i++)
			stats[membership[i]]++;
		for(int i=0;i<numClusters;i++){
			printf("I = %d", i);
			if(stats[i] != 0)
				printf("\t%d\t( %.2f %% )\n", stats[i], stats[i]*100.0/numObjs);
			else
				printf("\n");
		}
		
		free(stats);

		loop++;
		printf("\r\tcompleted loop %d", loop);
		fflush(stdout);
	} while (delta > threshold && loop < loop_threshold);

	for (k=0; k<nthreads; k++)
	{
		free(local_newClusterSize[k]);
		free(local_newClusters[k]);
	}
	free(newClusters);
	free(newClusterSize);

	int * stats = (typeof(stats)) calloc(numClusters, sizeof(*stats));
	for(int i=0;i<numObjs;i++)
		stats[membership[i]]++;
	printf("\n");
	for(int i=0;i<numClusters;i++){
		printf("I = %d", i);
		if(stats[i] != 0)
			printf("\t%d\t( %.2f %% )\n", stats[i], stats[i]*100.0/numObjs);
		else
			printf("\n");
	}
	
	free(stats);
}

void kmeans2_csc(int   * cc_r,             /* in: [numVals] */
				 int   * cc_c,             /* in: [numObjs] */
				 float * cc_v,             /* in: [numVals] */
				 int     numCoords,        /* no. coordinates */
				 int     numObjs,          /* no. objects */
				 int     numClusters,      /* no. clusters */
				 float   threshold,        /* minimum fraction of objects that change membership */
				 int     loop_threshold,   /* maximum number of iterations */
				 int   * membership,       /* out: [numObjs] */
				 float * clusters,         /* out: [numClusters][numCoords] */
				 int type)                 /* 0: euclidean distance, 1: cosine similarity, 2: hamming distance */
{
	int i, j, k;
	int index, loop=0;

	float delta;          // fraction of objects whose clusters change in each loop 
	int * newClusterSize; // [numClusters]: no. objects assigned in each new cluster 
	float * newClusters;  // [numClusters][numCoords] 
	int nthreads;         // no. threads 

	nthreads = omp_get_max_threads();
	// printf("OpenMP Kmeans - Reduction\t(number of threads: %d)\n", nthreads);

	// initialize membership
	for (i=0; i<numObjs; i++)
		membership[i] = -1;

	// initialize newClusterSize and newClusters to all 0 
	newClusterSize = (typeof(newClusterSize)) calloc(numClusters, sizeof(*newClusterSize));
	newClusters = (typeof(newClusters))  calloc(numClusters * numCoords, sizeof(*newClusters));

	// Each thread calculates new centers using a private space. After that, thread 0 does an array reduction on them.
	int * local_newClusterSize[nthreads];  // [nthreads][numClusters] 
	float * local_newClusters[nthreads];   // [nthreads][numClusters][numCoords]

	// Initialize local (per-thread) arrays (and later collect result on global arrays)
	#pragma omp parallel for
	for (k=0; k<nthreads; k++)
	{
		local_newClusterSize[k] = (typeof(*local_newClusterSize)) calloc(numClusters, sizeof(**local_newClusterSize));
		local_newClusters[k] = (typeof(*local_newClusters)) calloc(numClusters * numCoords, sizeof(**local_newClusters));
	}

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
		shared(cc_r,cc_c,cc_v,clusters,membership,newClusters,newClusterSize)
		{
			int tid = omp_get_thread_num();
			for (i=0; i<numClusters; i++){
				for (j=0; j<numCoords; j++)
					local_newClusters[tid][i*numCoords + j] = 0.0;
				local_newClusterSize[tid][i] = 0;
			}

			#pragma omp for schedule(static) reduction(+:delta)
			for (i=0; i<numObjs; i++)
			{
				float * local_object = (typeof(local_object)) calloc(numCoords, sizeof(*local_object));
				for(j=cc_c[i]; j<cc_c[i+1]; j++)
					local_object[cc_r[j]] = cc_v[j];
				// find the array index of nearest cluster center 
				// index = find_nearest_cluster(numClusters, numCoords, &objects[i*numCoords], clusters, type);
				index = find_nearest_cluster(numClusters, numCoords, local_object, clusters, type);
				
				// if membership changes, increase delta by 1 
				if (membership[i] != index)
					delta += 1.0;
				
				// assign the membership to object i 
				membership[i] = index;

				// update new cluster centers : sum of all objects located within (average will be performed later) 
				tid = omp_get_thread_num();
				local_newClusterSize[tid][index]++;
				for (j=0; j<numCoords; j++)
					// local_newClusters[tid][index*numCoords + j] += objects[i*numCoords + j];
					local_newClusters[tid][index*numCoords + j] += local_object[j];
				free(local_object);
			}

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
				for (j=0; j<numCoords; j++) {
					clusters[i*numCoords + j] = newClusters[i*numCoords + j] / newClusterSize[i];
				}
			}
		}

		printf("\tloop = %d, delta = %lf\n", loop, delta);

		// Get fraction of objects whose membership changed during this loop. This is used as a convergence criterion.
		delta /= numObjs;
		
		int * stats = (typeof(stats)) calloc(numClusters, sizeof(*stats));
		for(int i=0;i<numObjs;i++)
			stats[membership[i]]++;
		for(int i=0;i<numClusters;i++){
			printf("I = %d", i);
			if(stats[i] != 0)
				printf("\t%d\t( %.2f %% )\n", stats[i], stats[i]*100.0/numObjs);
			else
				printf("\n");
		}
		
		free(stats);

		loop++;
		printf("\r\tcompleted loop %d", loop);
		fflush(stdout);
	} while (delta > threshold && loop < loop_threshold);

	for (k=0; k<nthreads; k++)
	{
		free(local_newClusterSize[k]);
		free(local_newClusters[k]);
	}
	free(newClusters);
	free(newClusterSize);

	int * stats = (typeof(stats)) calloc(numClusters, sizeof(*stats));
	for(int i=0;i<numObjs;i++)
		stats[membership[i]]++;
	printf("\n");
	for(int i=0;i<numClusters;i++){
		printf("I = %d", i);
		if(stats[i] != 0)
			printf("\t%d\t( %.2f %% )\n", stats[i], stats[i]*100.0/numObjs);
		else
			printf("\n");
	}
	
	free(stats);
}
