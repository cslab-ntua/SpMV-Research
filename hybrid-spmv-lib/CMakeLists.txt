cmake_minimum_required (VERSION 3.10)
#set(CMAKE_VERBOSE_MAKEFILE on)

#	c) auto

#	d) auto

## 2) CUCUBLAS TODO: Other backends?
set (CUDA_PREFIX "/usr/local/cuda")

# 	a) Compiler
set (CMAKE_CUDA_COMPILER ${CUDA_PREFIX}/bin/nvcc)

# 	b) Compile flags
set (CMAKE_CUDA_FLAGS "-w -O3 -arch=sm_70") #35 for Tesla K40, 70 for V100, 61 for GTX 1060/70
message("CMAKE_CUDA_FLAGS: " ${CMAKE_CUDA_FLAGS})

## 4) ITER run for benchmark results
add_definitions(-DNR_ITER=1280)

## 5) Define full DDEBUG
#add_definitions(-DDDEBUG)

## 6) Define result testing flag
add_definitions(-DTEST)

project (MakeSpMVGreatAgain CXX CUDA)
set ( PROJECT_TEST_DIR ${PROJECT_SOURCE_DIR}/tests )
set ( PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src )
message( "PROJECT_TEST_DIR: " ${PROJECT_TEST_DIR} )
message( "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR} )
message( "PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR} )
message( "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR} )
message( "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR} )

##Mpakos Python flags
set (MPAKOS_PYTHON_CFLAGS "-I/usr/include/python3.6m")
message( "MPAKOS_PYTHON_CFLAGS: " ${MPAKOS_PYTHON_CFLAGS} )

##Mpakos C flags
set (MPAKOS_CFLAGS "-g -Wall -Wno-unused-result -Wsign-compare -Wformat -Werror=format-security -DNDEBUG -fwrapv -fstack-protector" )
message( "MPAKOS_CFLAGS: " ${MPAKOS_CFLAGS} )

# C++ compile flags
#set(CMAKE_CXX_STANDARD 11) # For dungani
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -O3 -fopenmp -lm ${MPAKOS_CFLAGS} ${MPAKOS_PYTHON_CFLAGS}") # -std=gnu89 TODO:needed? 
message( "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})

execute_process(COMMAND python3-config --ldflags OUTPUT_VARIABLE PYTHON_LINK OUTPUT_STRIP_TRAILING_WHITESPACE)
message( "PYTHON_LINK: " ${PYTHON_LINK} )

## CUDA libs and includes
set (CUDA_INCLUDE_DIRS ${CUDA_PREFIX}/include ${CUDA_PREFIX}/samples/common/inc)
include_directories(${CUDA_INCLUDE_DIRS} )
message( "CUDA_INCLUDE_DIRS: " ${CUDA_INCLUDE_DIRS} )
set (CUDA_LD "${CUDA_PREFIX}/lib64,${CUDA_PREFIX}/lib")
set (CUDA_LINK "-lcudart -lcublas -lcusparse") # -L${CUDA_LD} 
message( "CUDA_LINK: " ${CUDA_LINK} )

set(NVMLAPI_PREFIX "/home/users/panastas/PhD_stuff/SpMV-Research/nvml-power")
set (NVMLAPI_INCLUDE_DIRS ${NVMLAPI_PREFIX}) # -L${CUDA_LD} 
include_directories(${NVMLAPI_INCLUDE_DIRS} )
message( "NVMLAPI_INCLUDE_DIRS: " ${NVMLAPI_INCLUDE_DIRS} )

set (NVMLAPI_LINK "-L${CUDA_LD} -lnvidia-ml -L/usr/lib64 -lcuda -lpthread ${PYTHON_LINK}") # -L${CUDA_LD} 
message( "NVMLAPI_LINK: " ${NVMLAPI_LINK} )

# My header files 
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/inc)
message( "My header files: " ${CMAKE_CURRENT_SOURCE_DIR}/inc )

# Mpakos header files 
set (MPAKOS_PREFIX "/various/pmpakos/artificial_matrix_generation")
include_directories( ${MPAKOS_PREFIX}/include)
message( "Mpakos header files: " ${MPAKOS_PREFIX}/include )

# cuSP header files
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/inc/cuSP)
message( "cuSP header files: " ${CMAKE_CURRENT_SOURCE_DIR}/inc/cuSP )

# csr5 cuda header files
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/inc/CSR5/CSR5_cuda)
message( "CSR5_cuda header files: "  ${CMAKE_CURRENT_SOURCE_DIR}/inc/CSR5/CSR5_cuda)

# csr5 avx512 header files
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/inc/CSR5/CSR5_avx512)
message( "CSR5_avx512 header files: "  ${CMAKE_CURRENT_SOURCE_DIR}/inc/CSR5/CSR5_avx512)

# merge-spmv header files
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../merge-spmv)
message( "merge-spmv header files: "  ${CMAKE_CURRENT_SOURCE_DIR}/../merge-spmv)

set (INTERNAL_LINK_LIB "${CUDA_LINK} ${NVMLAPI_LINK} ${PYTHON_LINK} -lrt -ldl")
message( "INTERNAL_LINK_LIB: " ${INTERNAL_LINK_LIB})

file(GLOB SOURCE_FILES ${PROJECT_SOURCE_DIR}/*.c* ${NVMLAPI_PREFIX}/*.cpp)
message( "SOURCE_FILES: " ${SOURCE_FILES} )
add_library(MyLib ${SOURCE_FILES})
target_link_libraries(MyLib ${INTERNAL_LINK_LIB})

set (LINK_LIB "${CUDA_LINK} -lrt -ldl ${PYTHON_LINK}")
message( "LINK_LIB: " ${LINK_LIB} )

## datatype used in benchmarks
add_definitions(-DVALUE_TYPE=double)
add_executable (MakeDSpMVGreatAgain ${PROJECT_TEST_DIR}/MakeSpMVGreatAgain.cu)
target_link_libraries(MakeDSpMVGreatAgain ${LINK_LIB} MyLib)

## datatype used in benchmarks
#add_definitions(-DVALUE_TYPE=float)
#add_executable (MakeSSpMVGreatAgain ${PROJECT_TEST_DIR}/MakeSpMVGreatAgain.cu)
#target_link_libraries(MakeSSpMVGreatAgain ${LINK_LIB} MyLib)
