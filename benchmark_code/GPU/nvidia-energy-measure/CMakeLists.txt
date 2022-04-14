cmake_minimum_required (VERSION 3.10)
#set(CMAKE_VERBOSE_MAKEFILE on)

## 1) CUDA PREFIX
set (CUDA_TOOLKIT_ROOT_DIR "/usr/local/cuda-11.0")

# 	a) Compiler
set (CMAKE_CUDA_COMPILER ${CUDA_TOOLKIT_ROOT_DIR}/bin/nvcc)

# 	b) Compile flags
set (CMAKE_CUDA_FLAGS "-w -O3") # -arch=sm_70 35 for Tesla K40, 70 for V100, 61 for GTX 1060/70
message("CMAKE_CUDA_FLAGS: " ${CMAKE_CUDA_FLAGS})

#	c) Architecture 
if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
  set(CMAKE_CUDA_ARCHITECTURES 70) #35 for Tesla K40, 70 for V100, 61 for GTX 1060/70
endif()

project (nvidia-powa CXX CUDA)


# C++ compile flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -O3 -lm")
message( "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})

## CUDA libs and includes
set (CUDA_INCLUDE_DIRS ${CUDA_TOOLKIT_ROOT_DIR}/include ${CUDA_TOOLKIT_ROOT_DIR}/samples/common/inc)
include_directories(${CUDA_INCLUDE_DIRS} )
message( "CUDA_INCLUDE_DIRS: " ${CUDA_INCLUDE_DIRS} )
set (CUDA_LD "${CUDA_TOOLKIT_ROOT_DIR}/lib64,${CUDA_TOOLKIT_ROOT_DIR}/lib")
set (CUDA_LINK "-lcudart -lcublas -lcusparse") #
message( "CUDA_LINK: " ${CUDA_LINK} )

set(NVMLAPI_PREFIX ${PROJECT_SOURCE_DIR})
set (NVMLAPI_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src )
set (NVMLAPI_INCLUDE_DIRS ${NVMLAPI_PREFIX}/include) # -L${CUDA_LD} 
include_directories(${NVMLAPI_INCLUDE_DIRS} )
message( "NVMLAPI_INCLUDE_DIRS: " ${NVMLAPI_INCLUDE_DIRS} )

set (NVMLAPI_LINK "-L${CUDA_LD} -lnvidia-ml -L/usr/lib64 -lcuda -lpthread") # -L${CUDA_LD} 
message( "NVMLAPI_LINK: " ${NVMLAPI_LINK} )

set (INTERNAL_LINK_LIB "${CUDA_LINK} ${NVMLAPI_LINK} -lrt -ldl")
message( "INTERNAL_LINK_LIB: " ${INTERNAL_LINK_LIB})

file(GLOB SOURCE_FILES ${NVMLAPI_SOURCE_DIR}/*.c*)
message( "SOURCE_FILES: " ${SOURCE_FILES} )
add_library(nvidia_powa ${SOURCE_FILES})
target_link_libraries(nvidia_powa ${INTERNAL_LINK_LIB})

set (LINK_LIB "${CUDA_LINK} -lrt -ldl")
message( "LINK_LIB: " ${LINK_LIB} )

add_executable (example ${NVMLAPI_PREFIX}/example_main.cu)
target_link_libraries(example ${LINK_LIB} nvidia_powa)

