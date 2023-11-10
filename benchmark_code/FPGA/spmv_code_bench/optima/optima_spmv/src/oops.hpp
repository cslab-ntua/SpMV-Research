#pragma once 

#include "xcl2.hpp"

// this undefine had to happen when trying to compile in gold1
#undef binary_search
#include "ap_int.h"

#include <pthread.h>
#include <signal.h>

typedef ap_uint<32>  ColType;
typedef ap_uint<1> RowType;

#define MAX_CU 16

/*
// function that programs device with xclbin file
void program_device_start(cl::CommandQueue *queue, cl::Context *context, cl::Program *program){
	cl_int err;

	//programm_device(arg1,&err,&context, &q, &krnl,krnl_name);
	// auto binaryFile = arg1;

	// OPENCL HOST CODE AREA START
	auto devices = xcl::get_xil_devices();

	// Create Program and Kernel
	// auto fileBuf = xcl::read_binary_file(binaryFile);
	auto fileBuf = xcl::read_binary_file((char *) getenv("XCLBIN_FILENAME"));
	cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
	bool valid_device = false;
	for (unsigned int i = 0; i < devices.size(); i++) {
		auto device = devices[i];
		// int res = device.getInfo<CL_DEVICE_NAME>().compare("xilinx_u280_xdma_201920_3");
		// int res = device.getInfo<CL_DEVICE_NAME>().compare("xilinx_u55c_gen3x16_xdma_base_3");
		char *vitis_platform = (char *)malloc(100*sizeof(char));
		vitis_platform = (char *) getenv("VITIS_PLATFORM");
		// printf("vitis_platform = %s\n", vitis_platform);
		int res = device.getInfo<CL_DEVICE_NAME>().compare(vitis_platform);
		if(res==0){
			// Creating Context and Command Queue for selected Device
			OCL_CHECK(err, *context = cl::Context(device, nullptr, nullptr, nullptr, &err));
			OCL_CHECK(err, *queue = cl::CommandQueue(*context, device, CL_QUEUE_PROFILING_ENABLE, &err));
			std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
			*program=cl::Program(*context, {device}, bins, nullptr, &err);
		
			if (err != CL_SUCCESS) {
				std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
			}
			else
			{
				std::cout << "Device[" << i << "]: program successful!\n";
				valid_device = true;
				break; // we break because we found a valid device
			}
		}
	}

	if (!valid_device) {
		std::cout << "Failed to program any device found, exit!\n";
		exit(EXIT_FAILURE);
	}
}
*/

/****************************************************/
// function that programs device with xclbin file
std::vector<cl::Kernel> program_device(cl::CommandQueue *queue, cl::Context *context, cl::Program *program, cl::Kernel *kernels)
{
	std::vector<cl::Device> devices;
	cl::Device device;
	std::vector<cl::Platform> platforms;
	bool found_device = false;
	cl_int err;

	
	//traversing all Platforms To find Xilinx Platform and targeted
	//Device in Xilinx Platform
	cl::Platform::get(&platforms);
	int d_id = -1;
	for(size_t i = 0; (i < platforms.size() ) & (found_device == false) ;i++){
		cl::Platform platform = platforms[i];
		std::string platformName = platform.getInfo<CL_PLATFORM_NAME>();
		if ( platformName == "Xilinx"){
			devices.clear();
			platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);
			for (unsigned int j = 0; j < devices.size(); j++) {
				device = devices[j];
				char *vitis_platform = (char *)malloc(100*sizeof(char));
				vitis_platform = (char *) getenv("VITIS_PLATFORM");
				// printf("vitis_platform = %s\n", vitis_platform);
				int res = device.getInfo<CL_DEVICE_NAME>().compare(vitis_platform);
				// int res = device.getInfo<CL_DEVICE_NAME>().compare("xilinx_u280_xdma_201920_3");
				// int res = device.getInfo<CL_DEVICE_NAME>().compare("xilinx_u55c_gen3x16_xdma_base_3");
				if(res==0){
					d_id = j;
					devices[0] = devices[d_id];
					found_device = true;
					break;
				}
			}
		}
	}

	if (found_device == false)
		printf("Error: Unable to find Target Device\n");

	// Creating Context and 
	*context = cl::Context(device);
	*queue = cl::CommandQueue(*context, device, CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);

	// Load xclbin 
	char *xclbinFilename = (char *)malloc(250*sizeof(char));
	xclbinFilename = (char *) getenv("XCLBIN_FILENAME");
	// printf("Loading: %s\n", xclbinFilename);
	std::ifstream bin_file(xclbinFilename, std::ifstream::binary);
	bin_file.seekg (0, bin_file.end);
	unsigned nb = bin_file.tellg();
	bin_file.seekg (0, bin_file.beg);
	char *buf = new char [nb];
	bin_file.read(buf, nb);

	// Creating Program from Binary File
	cl::Program::Binaries bins;
	bins.push_back({buf,nb});
	devices.resize(1);
	*program = cl::Program(*context, devices, bins);

	// This call will get the kernel object from program. A kernel is an OpenCL function that is executed on the FPGA.
	// std::string krnl_name = "krnl_spmv";
	std::string krnl_name = getenv("KERNEL_NAME");
	int nstripe = atoi(getenv("CU"));
	std::vector<cl::Kernel> kernels(nstripe);
	for (int i = 0; i < nstripe; i++) {
		std::string krnl_name_full = krnl_name + ":{" + krnl_name + "_" + std::to_string(i + 1) + "}";
		printf("Creating a kernel [%s] for CU(%d)\n", krnl_name_full.c_str(), i);
		OCL_CHECK(err, kernels[i] = cl::Kernel(*program, krnl_name_full.c_str(), &err));
	}
	return kernels;
}

/****************************************************/

// OOPS memory allocation. Allocates aligned memory based on OS page size
// void *OOPS_malloc(size_t alloc_bytes);
// OOPS memory allocation. Allocates aligned memory based on OS page size
void *OOPS_malloc(size_t alloc_bytes){
	void *X;

	auto align_sz = sysconf(_SC_PAGESIZE);
	X= (void *)aligned_alloc(align_sz,alloc_bytes);

	return X;
}

/****************************************************/

bool isBufferAligned(cl::Buffer buffer, size_t alignment) {
	void* bufferPtr = buffer();
	uintptr_t bufferAddress = reinterpret_cast<uintptr_t>(bufferPtr);
	return (bufferAddress % alignment == 0);
}

/****************************************************/

int bin_search(const int ii, int iend, const int *const v){
	int istart = 0;
	int pos = iend/2;
	while (iend-istart > 1){
		if (v[pos] > ii){
			// Get the left interval
			iend = pos;
		} else {
			// Get the right interval
			istart = pos;
		}
		pos = (istart+iend)/2;
	}
	return pos;
}

#undef SWAP // it was defined already in lib/macros/macrolib.h
template <typename TYPE>
inline void SWAP(TYPE &i1, TYPE &i2){TYPE tmp = i1; i1 = i2; i2 = tmp;}
template void SWAP<int>(int &, int & );
template void SWAP<ValueType>(ValueType &, ValueType & );

void heapsort_2v(int* __restrict__ x1, ValueType* __restrict__ x2, const int n)
{
	for (int node = 1; node < n; node ++){
		int i = node;
		int j = i/2;
		while( x1[j] < x1[i] ){
			SWAP(x1[j],x1[i]);
			SWAP(x2[j],x2[i]);
			i = j;
			j = i/2;
			if (i == 0) break;
		}
	}
	
	for (int i = n-1; i > 0; i --){
		SWAP(x1[i],x1[0]);
		SWAP(x2[i],x2[0]);
		int k = i - 1;
		int ik = 0; 
		int jk = 1;
		if (k >= 2){
			if (x1[2] > x1[1]) jk = 2;
		}
		bool cont_cycle = false;
		if (jk <= k){
			if (x1[jk] > x1[ik]) cont_cycle = true;
		}
		while (cont_cycle){
			SWAP(x1[jk],x1[ik]);
			SWAP(x2[jk],x2[ik]);
			ik = jk;
			jk = ik*2;
			if (jk+1 <= k){
				if (x1[jk+1] > x1[jk]) jk = jk+1;
			}
			cont_cycle = false;
			if (jk <= k){
				if (x1[jk] > x1[ik]) cont_cycle = true;
			}
		}
	}
}
/****************************************************/
