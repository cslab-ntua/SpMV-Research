#ifndef NVEM_H
#define NVEM_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <nvml.h>
#include <pthread.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <time.h>
#include <unistd.h>

typedef struct nvem_results{
	char name[256];
	double total_bench_t;
	int sensor_ticks; 
	double W_avg;
	double J_estimated;
}* NvemStats_p;

typedef struct nvem_str{
	int verbose;
	int pollThreadStatus;
	pthread_t powerPollThread;
	char* PCIBusId;
	nvmlDevice_t device_handle;
	char* f_out;
	NvemStats_p stats;
}* NvemControl_p; 

void NvemStartMeasure(int dev_id, char* f_out, int verbose_mode);
NvemStats_p NvemStopMeasure(int dev_id, const char* name);

#endif
