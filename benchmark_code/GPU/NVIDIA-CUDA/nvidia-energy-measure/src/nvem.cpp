///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
/// Many thanks to Kajal Varma for providing the base code for this project (github.com/kajalv/nvml-power)
/// \brief Some CUDA function calls with added error-checking
///

#include "nvem.hpp"
#include "nvem_helpers.hpp"
#include <string.h>

nvmlEnableState_t pmmode;
nvmlComputeMode_t computeMode;

NvemControl_p* nvem_dev_stats = NULL;

/*
Return a number with a specific meaning. This number needs to be interpreted and handled appropriately.
*/
int getNVMLError(nvmlReturn_t resultToCheck)
{
	if (resultToCheck == NVML_ERROR_UNINITIALIZED)
		return 1;
	if (resultToCheck == NVML_ERROR_INVALID_ARGUMENT)
		return 2;
	if (resultToCheck == NVML_ERROR_NOT_SUPPORTED)
		return 3;
	if (resultToCheck == NVML_ERROR_NO_PERMISSION)
		return 4;
	if (resultToCheck == NVML_ERROR_ALREADY_INITIALIZED)
		return 5;
	if (resultToCheck == NVML_ERROR_NOT_FOUND)
		return 6;
	if (resultToCheck == NVML_ERROR_INSUFFICIENT_SIZE)
		return 7;
	if (resultToCheck == NVML_ERROR_INSUFFICIENT_POWER)
		return 8;
	if (resultToCheck == NVML_ERROR_DRIVER_NOT_LOADED)
		return 9;
	if (resultToCheck == NVML_ERROR_TIMEOUT)
		return 10;
	if (resultToCheck == NVML_ERROR_IRQ_ISSUE)
		return 11;
	if (resultToCheck == NVML_ERROR_LIBRARY_NOT_FOUND)
		return 12;
	if (resultToCheck == NVML_ERROR_FUNCTION_NOT_FOUND)
		return 13;
	if (resultToCheck == NVML_ERROR_CORRUPTED_INFOROM)
		return 14;
	if (resultToCheck == NVML_ERROR_GPU_IS_LOST)
		return 15;
	if (resultToCheck == NVML_ERROR_UNKNOWN)
		return 16;

	return 0;
}

/*
Poll the GPU using nvml APIs.
*/
void *powerPollingFunc(void *curr_dev_stats_v)
{
	NvemControl_p curr_dev_stats = (NvemControl_p) curr_dev_stats_v;
	unsigned int powerLevel = 0;
	FILE *fp = fopen(curr_dev_stats->f_out, "w+");
	if(!fp) warning("powerPollingFunc: error in opening file=%s, will not log power measurements to file\n", curr_dev_stats->f_out);

	double first_timer = csecond();
	while (curr_dev_stats->pollThreadStatus)
	{

		double timer = csecond();

		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);

		// Get the power management mode of the GPU.
		nvmlReturn_t nvmlResult = nvmlDeviceGetPowerManagementMode(curr_dev_stats->device_handle, &pmmode);

		// The following function may be utilized to handle errors as needed.
		getNVMLError(nvmlResult);

		// Check if power management mode is enabled.
		if (pmmode == NVML_FEATURE_ENABLED) nvmlResult = nvmlDeviceGetPowerUsage(curr_dev_stats->device_handle, &powerLevel); // Get the power usage in milliWatts.
		else error("powerPollingFunc: Unable to poll device for power management data");

		timer = csecond() - timer;
		double WAT = powerLevel/1000.0;
		// The output file stores power in Watts.
		fprintf(fp, "%.3lf,%.3lf\n", timer*1000, WAT);
		curr_dev_stats->stats->total_bench_t+=timer;
		curr_dev_stats->stats->sensor_ticks++; 
		curr_dev_stats->stats->J_estimated+=WAT*timer;
		if(curr_dev_stats->verbose == 2) fprintf(stdout, "Sensor tick %d: time = %.3lf ms, Watts = %.3lf, J_estimated = %.3lf\n", 
				curr_dev_stats->stats->sensor_ticks, timer*1000, WAT, WAT*timer);
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
	}

	fclose(fp);
	pthread_exit(0);
}

void NvemInit(){
		int dev_num;
		cudaGetDeviceCount(&dev_num);
		nvem_dev_stats = (NvemControl_p*) malloc (dev_num * sizeof(NvemControl_p));
		for (int i = 0; i < dev_num; i++) nvem_dev_stats[i] = NULL;
		// Initialize nvml.
		nvmlReturn_t nvmlResult = nvmlInit();
		if (NVML_SUCCESS != nvmlResult) error("NvemStartMeasure: NVML Init fail: %s\n", nvmlErrorString(nvmlResult));
}
/*
Start power measurement by spawning a pthread that polls the GPU.
Function needs to be modified as per usage to handle errors as seen fit.
*/
void NvemStartMeasure(int dev_id, char* f_out, int verbose)
{ 	
	int dev_num;
	cudaGetDeviceCount(&dev_num);
	if (dev_id < 0 || dev_id >= dev_num) error("NvemStartMeasure: dev_id out of CUDA device bounds, power measuring will not commence\n");

	if (!nvem_dev_stats) NvemInit();

	if(nvem_dev_stats[dev_id]!=NULL) free(nvem_dev_stats[dev_id]);
	nvem_dev_stats[dev_id] = (NvemControl_p) malloc (sizeof(struct nvem_str));
	NvemControl_p curr_dev_stats = nvem_dev_stats[dev_id];
	curr_dev_stats->stats = (NvemStats_p) malloc(sizeof(struct nvem_results));
	curr_dev_stats->stats->total_bench_t = curr_dev_stats->stats->sensor_ticks = curr_dev_stats->stats->W_avg = curr_dev_stats->stats->J_estimated = 0;
	curr_dev_stats->PCIBusId = (char*) malloc(256*sizeof(char));
	curr_dev_stats->PCIBusId[255]='\0';
	curr_dev_stats->f_out = f_out;
	curr_dev_stats->pollThreadStatus = 0;
	curr_dev_stats->verbose = verbose;
	cudaDeviceProp deviceProp;
	cudaGetDeviceProperties(&deviceProp, dev_id);
	if(curr_dev_stats->verbose) fprintf(stdout, "NvemStartMeasure: Device [%s] - Bus ID num: %d\n", deviceProp.name , deviceProp.pciBusID);

	cudaDeviceGetPCIBusId(curr_dev_stats->PCIBusId, 255, dev_id);
    if(curr_dev_stats->verbose) fprintf(stdout, "NvemStartMeasure: Device Bus ID : %s\n", curr_dev_stats->PCIBusId);

	// Count the number of GPUs available.
	unsigned int deviceCount;
	nvmlReturn_t nvmlResult = nvmlDeviceGetCount(&deviceCount);
	if(curr_dev_stats->verbose) fprintf(stdout, "NvemStartMeasure: Found %d nvml devices\n", deviceCount);
	if (NVML_SUCCESS != nvmlResult) warning("Failed to query device count: %s\n", nvmlErrorString(nvmlResult));

	nvmlResult = nvmlDeviceGetHandleByPciBusId_v2(curr_dev_stats->PCIBusId, &curr_dev_stats->device_handle);
	if(curr_dev_stats->verbose) fprintf(stdout, "NvemStartMeasure: Got handle for device with Bus ID %s\n", curr_dev_stats->PCIBusId);
	// Get the compute mode of the device which indicates CUDA capabilities.
	nvmlResult = nvmlDeviceGetComputeMode(curr_dev_stats->device_handle, &computeMode);
	if (NVML_ERROR_NOT_SUPPORTED == nvmlResult) error("NvemStartMeasure: GPU with dev_id=%d is probably not a CUDA-capable device -\
		NVML_ERROR_NOT_SUPPORTED returned when quering for compute mode.\n", dev_id);
	else if (NVML_SUCCESS != nvmlResult) error("NvemStartMeasure: Failed to get compute mode for device %d: %s\n", dev_id, nvmlErrorString(nvmlResult));
	else if (curr_dev_stats->verbose) fprintf(stdout, "NvemStartMeasure: GPU with dev_id=%d is a CUDA-capable device\n", dev_id);
	curr_dev_stats->pollThreadStatus = 1;

	int iret = pthread_create(&curr_dev_stats->powerPollThread, NULL, powerPollingFunc, (void*) curr_dev_stats);
	if (iret) error("pthread_create() return code: %d\n", iret);

}

/*
End power measurement. This ends the polling thread.
*/
NvemStats_p NvemStopMeasure(int dev_id, const char* name)
{
	NvemControl_p curr_dev_stats = nvem_dev_stats[dev_id];
	NvemStats_p out_stats = curr_dev_stats->stats;
	strcpy(out_stats->name, name);
	curr_dev_stats->pollThreadStatus = 0;
	pthread_join(curr_dev_stats->powerPollThread, NULL);
	if(out_stats->sensor_ticks > 0) out_stats->W_avg = out_stats->J_estimated/out_stats->total_bench_t;
	else warning("NvemStopMeasure: No sensor ticks were logged, try with larger itter/kernel ( sensor tick frequency varies in the ~ms area)\n");
	if(curr_dev_stats->verbose) fprintf(stdout, "NvemStopMeasure: Completed measurements \"%s\": Total ticks = %d, Elapsed_time = %.3lf s, Av. Watts = %.3lf, J_total_estimated = %.3lf\n", 
				out_stats->name, out_stats->sensor_ticks, out_stats->total_bench_t, out_stats->W_avg, out_stats->J_estimated);
	return out_stats;
}

void NvemShutdown(){
	nvmlReturn_t nvmlResult = nvmlShutdown();
	if (NVML_SUCCESS != nvmlResult) error("Failed to shut down NVML: %s\n", nvmlErrorString(nvmlResult));
}

