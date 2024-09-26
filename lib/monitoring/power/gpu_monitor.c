#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "gpu_monitor.h"

// #include <rocm_smi/rocm_smi.h>

void nvidia_gpu_monitor_open()
{
	// When called, this initializes internal data structures, including those corresponding to sources of information that SMI provides.
	// nvmlInit(0);
}

void nvidia_gpu_monitor_close()
{
	// nvmlShutdown();
}

double nvidia_gpu_monitor(uint32_t dv_ind, uint32_t sensor_ind)
{
	// nvmlDevice_t device;
	// uint64_t power=0;
	// nvmlDeviceGetHandleByIndex(dv_ind, &device);
	// nvmlDeviceGetPowerUsage(device, &power);
	// return (double)power/1e3;
	return 0;
}

void amd_gpu_monitor_open()
{
	// When called, this initializes internal data structures, including those corresponding to sources of information that SMI provides.
	rsmi_init(0);
}

void amd_gpu_monitor_close()
{
	// Shutdown ROCm SMI.
	rsmi_shut_down();
}

double amd_gpu_monitor(uint32_t dv_ind, uint32_t sensor_ind)
{
	uint64_t power=0;
	// returns in uWatt
	rsmi_dev_power_ave_get(dv_ind, sensor_ind, &power);
	// return in Watt
	return (double)power/1e6;
}
