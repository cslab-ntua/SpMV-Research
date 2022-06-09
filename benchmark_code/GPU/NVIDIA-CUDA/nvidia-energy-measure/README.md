# nvidia-energy-measure

A repository providing some useful wrappers for GPU energy measurement using CUDA/NVML.
The scripts are based on the power measurement code in github.com/kajalv/nvml-power, with some additions and changes for ease of use and customization. Many thanks to Kajal Varma for the baseline code!

The provided API uses NVML to give a basic estimation of power usage (similar to nvidia-smi) in a GPU device within a measurement area.
- A pthread is spawned for each **NvemStartMeasure** call, which queries power usage from the nvml sensor until **NvemStopMeasure** is called. 
- The results are as accurate as the nvml sensor output, which returns power measurements in varied time intervals in the order of milliseconds.
- GPUs also consume power while idle, which means a warmup before power measurement is adviced to emulate a 'hot' GPU. 
- **Therefore** the API can only provide useful insights for fairly big problems, which run in the order of seconds (or hundreds of ms). 

## Library Requirements

- CUDA toolkit. Tested with cuda-9.2 and 11.0
- The library files use a C++ compiler, but can also be compiled with nvcc (change to .cu).

## Cmake/example build Requirements

- Cmake >= 3.10

*Note: using the actual API does NOT require using the cmakefile to build the project.*


## Compiling and linking the API

###Using the Cmake generated libfile

- Change the *CUDA_TOOLKIT_ROOT_DIR* and *CMAKE_CUDA_ARCHITECTURES* in CMakeLists.txt
- Build the project
```sh
mkdir buildtest && cd buildtest && cmake ../ && make -j
```

###Building along with your code in a Makefile

- Add the following to the Makefile:

```sh
NVEM_ROOT_DIR=**root_location_of_this_repo**
CUDA_TOOLKIT_ROOT_DIR=**Path_to_your_cuda_location**
INCLUDECUDASAMPLES = -I$(CUDA_TOOLKIT_ROOT_DIR)/samples/common/inc/
HEADERNVEMAPI = -L/usr/lib64/nvidia -lnvidia-ml -L/usr/lib64 -lcuda -I/usr/include -lpthread -I/$(NVEM_ROOT_DIR)/include
NVEMSRC = $(NVEM_ROOT_DIR)/src/*.c*
```

- Add $(HEADERNVEMAPI) at the end of *nvcc* compilation commands.
- Include the module files while compiling the program, along with *INCLUDECUDASAMPLES*.

Example:
```sh
example_prog: example_prog.cu $(NVEMSRC)
	nvcc $(INCLUDECUDASAMPLES) -o $@ $+ $(HEADERNVEMAPI)
```

## Integrating to CUDA code

### Usage
- Include the *nvem.hpp* header.
- Surround the areas of interest with **NvemStartMeasure** and **NvemStopMeasure**.
- A detailed usage example can be found in example_main.cu

### Parameters/options
The user provides to the API via 
```sh
void NvemStartMeasure(int dev_id, char* f_out, int verbose_mode)
```
- The **CUDA** device ID in which the power measuring will commence. 
- A filename for result printing ( *note: the API writes (w+) values to this file, therefore multiple calls using the same f_out will replace results - use another file* ).
- a verbose flag argument ( 0 : no extra stdout info, !0 : extra printing for API steps/usage, 2 : detailed log of all queried vaues from sensor.

The API returns results with
```sh
NvemStats_p NvemStopMeasure(int dev_id, const char* name)
```
- The API computes and returns the **total time** measured, the **average Watts** for this duration and the estimated **Joules** from these values. The results are stored in 
```sh
typedef struct nvem_results{
	char name[256];
	double total_bench_t;
	int sensor_ticks; 
	double W_avg;
	double J_estimated;
}* NvemStats_p;
```

Note: using the actual API does NOT require using the cmakefile to build the project.

