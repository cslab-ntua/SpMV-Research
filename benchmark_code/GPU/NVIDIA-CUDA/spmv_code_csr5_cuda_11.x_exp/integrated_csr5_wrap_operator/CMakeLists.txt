cmake_minimum_required (VERSION 3.10)
#set(CMAKE_VERBOSE_MAKEFILE on)

## 1) CXX compiler stuff

#	a) auto

#	b) auto

## 2) CUDA Compiler stuff

set (CUDA_TOOLKIT_ROOT_DIR $ENV{cnf_CUDA_TOOLKIT_11_DIR})

# 	a) Compiler
set (CMAKE_CUDA_COMPILER ${CUDA_TOOLKIT_ROOT_DIR}/bin/nvcc)

# 	b) Compile flags
set (CMAKE_CUDA_FLAGS "-w -O3")
message("CMAKE_CUDA_FLAGS: " ${CMAKE_CUDA_FLAGS})

#	c) Architecture 
if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
  set(CMAKE_CUDA_ARCHITECTURES $ENV{cnf_CUDA_arch})
endif()

## 4) ITER run for benchmark results
add_definitions(-DNR_ITER=128)

## 5) Define full DDEBUG
#add_definitions(-DDDEBUG)

## 6) Define result testing flag
#add_definitions(-DTEST)

project (MakeSpMVGreatAgainCSR511 CXX CUDA)
set ( PROJECT_HEAD_DIR ${PROJECT_SOURCE_DIR}/../../../../../ )
set ( PROJECT_INC_DIR ${PROJECT_SOURCE_DIR}/../integrated_csr5_wrap_operator/inc )
set ( PROJECT_TEST_DIR ${PROJECT_SOURCE_DIR}/../integrated_csr5_wrap_operator/tests )
set ( PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../integrated_csr5_wrap_operator/src )
message( "PROJECT_HEAD_DIR: " ${PROJECT_HEAD_DIR} )
message( "PROJECT_INC_DIR: " ${PROJECT_INC_DIR} )
message( "PROJECT_TEST_DIR: " ${PROJECT_TEST_DIR} )
message( "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR} )
message( "PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR} )
message( "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR} )
message( "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR} )

# C++ compile flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -O3 -fopenmp -lm")
message( "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})

## CUDA libs and includes
set (CUDA_INCLUDE_DIRS ${CUDA_TOOLKIT_ROOT_DIR}/include ${CUDA_TOOLKIT_ROOT_DIR}/samples/common/inc)
include_directories(${CUDA_INCLUDE_DIRS} )
message( "CUDA_INCLUDE_DIRS: " ${CUDA_INCLUDE_DIRS} )
set (CUDA_LD "${CUDA_TOOLKIT_ROOT_DIR}/lib64,${CUDA_TOOLKIT_ROOT_DIR}/lib")
set (CUDA_LINK "-lcudart -lcublas -lcusparse") # -L${CUDA_LD} 
message( "CUDA_LINK: " ${CUDA_LINK} )

set(NVEMAPI_PREFIX "../../nvidia-energy-measure")
set (NVEMAPI_INCLUDE_DIRS ${NVEMAPI_PREFIX}/include) # -L${CUDA_LD} 
include_directories(${NVEMAPI_INCLUDE_DIRS} )
message( "NVEMAPI_INCLUDE_DIRS: " ${NVEMAPI_INCLUDE_DIRS} )

set (NVEMAPI_LINK "-L${CUDA_LD} -lnvidia-ml -L/usr/lib64 -lcuda -lpthread") #  ${PYTHON_LINK}
message( "NVEMAPI_LINK: " ${NVEMAPI_LINK} )

# My header files 
include_directories(${PROJECT_INC_DIR})
message( "My header files: " ${PROJECT_INC_DIR} )

# csr5 cuda header files
include_directories( ${PROJECT_SOURCE_DIR}/../../)
message( "csr5 header files: "  ${PROJECT_SOURCE_DIR}/../../)

# Artificial matrix generator header files 
set (AMG_PREFIX "${PROJECT_HEAD_DIR}/artificial-matrix-generator")
include_directories( ${AMG_PREFIX})
message( "DGAL header files: " ${AMG_PREFIX} )

set (INTERNAL_LINK_LIB "${CUDA_LINK} ${NVEMAPI_LINK} ${PYTHON_LINK} -lrt -ldl")
message( "INTERNAL_LINK_LIB: " ${INTERNAL_LINK_LIB})

set (LINK_LIB "${CUDA_LINK} -lrt -ldl ${PYTHON_LINK} ${NVEMAPI_LINK}")
message( "LINK_LIB: " ${LINK_LIB} )

file(GLOB SOURCE_FILES ${PROJECT_SOURCE_DIR}/*.c* ${NVEMAPI_PREFIX}/src/*.*)
message( "SOURCE_FILES: " ${SOURCE_FILES} )

## datatype used in benchmarks. 0 = float, 1 = double
set(DATATYPE $ENV{cnf_dtype_id}) 

if (${DATATYPE} EQUAL 1)
	set (DGAL_LINK "-L${AMG_PREFIX} -lartificial_matrix_generation_double")
	add_library(MyLib ${SOURCE_FILES})
	target_link_libraries(MyLib ${INTERNAL_LINK_LIB} ${DGAL_LINK})
	add_definitions(-DVALUE_TYPE_AX=double)
	add_definitions(-DVALUE_TYPE_Y=double)
	add_definitions(-DVALUE_TYPE_COMP=double)

	add_executable (CSR5_CUDA_DSPMV_11_mtx ${PROJECT_TEST_DIR}/testCudaCSR5_mtx.cu)
	target_link_libraries(CSR5_CUDA_DSPMV_11_mtx ${LINK_LIB} MyLib)

	add_executable (CSR5_CUDA_DSPMV_11_generate ${PROJECT_TEST_DIR}/testCudaCSR5_generate.cu)
	target_link_libraries(CSR5_CUDA_DSPMV_11_generate ${LINK_LIB} MyLib)
elseif(${DATATYPE} EQUAL 0)
	set (DGAL_LINK "-L${AMG_PREFIX} -lartificial_matrix_generation_float")
	add_library(MyLib ${SOURCE_FILES})
	target_link_libraries(MyLib ${INTERNAL_LINK_LIB} ${DGAL_LINK})
	add_definitions(-DVALUE_TYPE_AX=float)
	add_definitions(-DVALUE_TYPE_Y=float)
	add_definitions(-DVALUE_TYPE_COMP=float)

	add_executable (CSR5_CUDA_SSPMV_11_mtx ${PROJECT_TEST_DIR}/testCudaCSR5_mtx.cu)
	target_link_libraries(CSR5_CUDA_SSPMV_11_mtx ${LINK_LIB} MyLib)

	add_executable (CSR5_CUDA_SSPMV_11_generate ${PROJECT_TEST_DIR}/testCudaCSR5_generate.cu)
	target_link_libraries(CSR5_CUDA_SSPMV_11_generate ${LINK_LIB} MyLib)
else()

#add_definitions(-DVALUE_TYPE_AX=int8_t)
#add_definitions(-DVALUE_TYPE_Y=int32_t)
#add_definitions(-DVALUE_TYPE_COMP=int32_t)
#add_executable (cuSPARSEMIcsrmv_11-0_mtx ${PROJECT_TEST_DIR}/cuSPARSEcsrmv_11-0_mtx.cu)
#target_link_libraries(cuSPARSEMIcsrmv_11-0_mtx ${LINK_LIB} MyLib)
endif()
