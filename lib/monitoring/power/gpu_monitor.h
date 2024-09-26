#ifndef GPU_MONITOR_H
#define GPU_MONITOR_H

#include "macros/cpp_defines.h"

void nvidia_gpu_monitor_open();
void nvidia_gpu_monitor_close();
double nvidia_gpu_monitor(uint32_t dv_ind, uint32_t sensor_ind);

void amd_gpu_monitor_open();
void amd_gpu_monitor_close();
double amd_gpu_monitor(uint32_t dv_ind, uint32_t sensor_ind);

#endif /* GPU_MONITOR_H */

