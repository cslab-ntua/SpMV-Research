#compilers

#GLOBAL_PARAMETERS
VALUE_TYPE = double
NUM_RUN = 1000

#ENVIRONMENT_PARAMETERS

#CUDA_PARAMETERS
NVCC_FLAGS = -O3 -w -m64 -gencode=arch=compute_80,code=sm_80
CUDA_INSTALL_PATH = /usr/local/cuda
CUDA_INCLUDES = -I$(CUDA_INSTALL_PATH)/include -I/various/pmpakos/epyc5_libs/cuda-samples/Common
# CUDA_INCLUDES = -I$(CUDA_INSTALL_PATH)/include -I../../NVIDIA_CUDA-11.1_Samples/common/inc
CUDA_LIBS = -L$(CUDA_INSTALL_PATH)/lib64 -lcudart

cuda:
	${CUDA_INSTALL_PATH}/bin/nvcc $(NVCC_FLAGS) main.cu -o spmv $(CUDA_INCLUDES) $(CUDA_LIBS) -D VALUE_TYPE=$(VALUE_TYPE) -D NUM_RUN=$(NUM_RUN)
