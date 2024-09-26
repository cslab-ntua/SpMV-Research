# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# Escaping for special characters.
EQUALS = =

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = build

# Include the compile flags for this target's objects.
CXX_DEFINES = -DCSV_LOG -DUSE_GLOP

CXX_INCLUDES = -Ilib/or-tools/include -Iinclude -Isrc/trace_gen -Ilib/cxxopts/include -Ilib/aggregation/src/includes -Ilib/aggregation/include -Ilib/aggregation/external/metis/metis_shared-src/include -I/opt/intel/oneapi/mkl/latest/include -Ilib -Ilib/CSR5_LOCAL/CSR5_avx2 -Ilib/CSR5_LOCAL/CSR5_avx2/detail -Ilib/CSR5_LOCAL/CSR5_avx2/detail/avx2 -isystem build/_deps/metis-src/include

CXX_FLAGS =   -march=native -fopenmp -std=c++17  -march=native -O3 -DNDEBUG -std=gnu++17 -fopenmp


all: spmv_lcm_d.exe

##################################################################################################################
AMG_PATH = $(shell pwd)/../../../../../artificial-matrix-generator/
LIB_PATH = $(shell pwd)/../../../../../lib/

# AMG_PATH = /pfs/lustrep2/scratch/project_465000869/pmpakos/SpMV-Research/artificial-matrix-generator/
LIB_PATH = /pfs/lustrep2/scratch/project_465000869/pmpakos/SpMV-Research/lib/
AMG_CFLAGS=-D'DOUBLE=1'  -I'$(AMG_PATH)' -I'${LIB_PATH}' -g -O3 -pedantic -fopenmp -Wall -Wno-unused-local-typedefs
CXX_INCLUDES += -I${AMG_PATH} -I${LIB_PATH}

build/demo/CMakeFiles/spmv_demo.dir/artificial_matrix_generation.o: ${AMG_PATH}/artificial_matrix_generation.c
	$(CC) $(AMG_CFLAGS) -o build/demo/CMakeFiles/spmv_demo.dir/artificial_matrix_generation.o -c ${AMG_PATH}/artificial_matrix_generation.c

build/demo/CMakeFiles/spmv_demo.dir/ordered_set.o: ${AMG_PATH}/ordered_set.c
	$(CC) $(AMG_CFLAGS) -o build/demo/CMakeFiles/spmv_demo.dir/ordered_set.o -c ${AMG_PATH}/ordered_set.c
##################################################################################################################

build/demo/CMakeFiles/spmv_demo.dir/FusionDemo.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/FusionDemo.cpp.o: demo/FusionDemo.cpp
build/demo/CMakeFiles/spmv_demo.dir/FusionDemo.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/FusionDemo.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/FusionDemo.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/FusionDemo.cpp.o -c demo/FusionDemo.cpp

build/demo/CMakeFiles/spmv_demo.dir/SPMV_demo.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/SPMV_demo.cpp.o: demo/SPMV_demo.cpp
build/demo/CMakeFiles/spmv_demo.dir/SPMV_demo.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) -I${AMG_PATH} -I${LIB_PATH} $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/SPMV_demo.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/SPMV_demo.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/SPMV_demo.cpp.o -c demo/SPMV_demo.cpp

build/demo/CMakeFiles/spmv_demo.dir/SerialSpMVExecutor.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/SerialSpMVExecutor.cpp.o: demo/SerialSpMVExecutor.cpp
build/demo/CMakeFiles/spmv_demo.dir/SerialSpMVExecutor.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/SerialSpMVExecutor.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/SerialSpMVExecutor.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/SerialSpMVExecutor.cpp.o -c demo/SerialSpMVExecutor.cpp

build/demo/CMakeFiles/spmv_demo.dir/__/src/Analyzer.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/__/src/Analyzer.cpp.o: src/Analyzer.cpp
build/demo/CMakeFiles/spmv_demo.dir/__/src/Analyzer.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/__/src/Analyzer.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/__/src/Analyzer.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/__/src/Analyzer.cpp.o -c src/Analyzer.cpp

build/demo/CMakeFiles/spmv_demo.dir/__/src/DDT.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/__/src/DDT.cpp.o: src/DDT.cpp
build/demo/CMakeFiles/spmv_demo.dir/__/src/DDT.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/__/src/DDT.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/__/src/DDT.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/__/src/DDT.cpp.o -c src/DDT.cpp

build/demo/CMakeFiles/spmv_demo.dir/__/src/DDTUtils.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/__/src/DDTUtils.cpp.o: src/DDTUtils.cpp
build/demo/CMakeFiles/spmv_demo.dir/__/src/DDTUtils.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/__/src/DDTUtils.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/__/src/DDTUtils.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/__/src/DDTUtils.cpp.o -c src/DDTUtils.cpp

build/demo/CMakeFiles/spmv_demo.dir/__/src/Executor.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/__/src/Executor.cpp.o: src/Executor.cpp
build/demo/CMakeFiles/spmv_demo.dir/__/src/Executor.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/__/src/Executor.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/__/src/Executor.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/__/src/Executor.cpp.o -c src/Executor.cpp

build/demo/CMakeFiles/spmv_demo.dir/__/src/Input.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/__/src/Input.cpp.o: src/Input.cpp
build/demo/CMakeFiles/spmv_demo.dir/__/src/Input.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/__/src/Input.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/__/src/Input.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/__/src/Input.cpp.o -c src/Input.cpp

build/demo/CMakeFiles/spmv_demo.dir/__/src/Inspector.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/__/src/Inspector.cpp.o: src/Inspector.cpp
build/demo/CMakeFiles/spmv_demo.dir/__/src/Inspector.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/__/src/Inspector.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/__/src/Inspector.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/__/src/Inspector.cpp.o -c src/Inspector.cpp

build/demo/CMakeFiles/spmv_demo.dir/__/src/PatternMatching.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/__/src/PatternMatching.cpp.o: src/PatternMatching.cpp
build/demo/CMakeFiles/spmv_demo.dir/__/src/PatternMatching.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/__/src/PatternMatching.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/__/src/PatternMatching.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/__/src/PatternMatching.cpp.o -c src/PatternMatching.cpp

build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/PolyModel.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/PolyModel.cpp.o: src/trace_gen/PolyModel.cpp
build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/PolyModel.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/PolyModel.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/PolyModel.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/PolyModel.cpp.o -c src/trace_gen/PolyModel.cpp

build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/SpMVModel.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/SpMVModel.cpp.o: src/trace_gen/SpMVModel.cpp
build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/SpMVModel.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/SpMVModel.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/SpMVModel.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/SpMVModel.cpp.o -c src/trace_gen/SpMVModel.cpp

build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/Trace.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/Trace.cpp.o: src/trace_gen/Trace.cpp
build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/Trace.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/Trace.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/Trace.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/Trace.cpp.o -c src/trace_gen/Trace.cpp

build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/TraceIR.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/TraceIR.cpp.o: src/trace_gen/TraceIR.cpp
build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/TraceIR.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/TraceIR.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/TraceIR.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/TraceIR.cpp.o -c src/trace_gen/TraceIR.cpp

build/demo/CMakeFiles/spmv_demo.dir/__/lib/lbc_csc_dag.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/__/lib/lbc_csc_dag.cpp.o: lib/lbc_csc_dag.cpp
build/demo/CMakeFiles/spmv_demo.dir/__/lib/lbc_csc_dag.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/__/lib/lbc_csc_dag.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/__/lib/lbc_csc_dag.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/__/lib/lbc_csc_dag.cpp.o -c lib/lbc_csc_dag.cpp

build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsFSC.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsFSC.cpp.o: src/spmv_codelets/GenericCodeletsFSC.cpp
build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsFSC.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsFSC.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsFSC.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsFSC.cpp.o -c src/spmv_codelets/GenericCodeletsFSC.cpp

build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT1.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT1.cpp.o: src/spmv_codelets/GenericCodeletsPSCT1.cpp
build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT1.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT1.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT1.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT1.cpp.o -c src/spmv_codelets/GenericCodeletsPSCT1.cpp

build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT2.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT2.cpp.o: src/spmv_codelets/GenericCodeletsPSCT2.cpp
build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT2.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT2.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT2.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT2.cpp.o -c src/spmv_codelets/GenericCodeletsPSCT2.cpp

build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT3.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT3.cpp.o: src/spmv_codelets/GenericCodeletsPSCT3.cpp
build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT3.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT3.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT3.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT3.cpp.o -c src/spmv_codelets/GenericCodeletsPSCT3.cpp

build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/SpMVGenericCode.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/flags.make
build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/SpMVGenericCode.cpp.o: src/spmv_codelets/SpMVGenericCode.cpp
build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/SpMVGenericCode.cpp.o: build/demo/CMakeFiles/spmv_demo.dir/compiler_depend.ts
	$(CPP) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/SpMVGenericCode.cpp.o -MF build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/SpMVGenericCode.cpp.o.d -o build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/SpMVGenericCode.cpp.o -c src/spmv_codelets/SpMVGenericCode.cpp

# Object files for target spmv_demo
spmv_demo_OBJECTS = \
"CMakeFiles/spmv_demo.dir/artificial_matrix_generation.o" \
"CMakeFiles/spmv_demo.dir/ordered_set.o" \
"CMakeFiles/spmv_demo.dir/FusionDemo.cpp.o" \
"CMakeFiles/spmv_demo.dir/SPMV_demo.cpp.o" \
"CMakeFiles/spmv_demo.dir/SerialSpMVExecutor.cpp.o" \
"CMakeFiles/spmv_demo.dir/__/src/Analyzer.cpp.o" \
"CMakeFiles/spmv_demo.dir/__/src/DDT.cpp.o" \
"CMakeFiles/spmv_demo.dir/__/src/DDTUtils.cpp.o" \
"CMakeFiles/spmv_demo.dir/__/src/Executor.cpp.o" \
"CMakeFiles/spmv_demo.dir/__/src/Input.cpp.o" \
"CMakeFiles/spmv_demo.dir/__/src/Inspector.cpp.o" \
"CMakeFiles/spmv_demo.dir/__/src/PatternMatching.cpp.o" \
"CMakeFiles/spmv_demo.dir/__/src/trace_gen/PolyModel.cpp.o" \
"CMakeFiles/spmv_demo.dir/__/src/trace_gen/SpMVModel.cpp.o" \
"CMakeFiles/spmv_demo.dir/__/src/trace_gen/Trace.cpp.o" \
"CMakeFiles/spmv_demo.dir/__/src/trace_gen/TraceIR.cpp.o" \
"CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsFSC.cpp.o" \
"CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT1.cpp.o" \
"CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT2.cpp.o" \
"CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT3.cpp.o" \
"CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/SpMVGenericCode.cpp.o" \


##################################################################################################################
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/artificial_matrix_generation.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/ordered_set.o
##################################################################################################################
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/FusionDemo.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/SPMV_demo.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/SerialSpMVExecutor.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/__/src/Analyzer.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/__/src/DDT.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/__/src/DDTUtils.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/__/src/Executor.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/__/src/Input.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/__/src/Inspector.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/__/src/PatternMatching.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/PolyModel.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/SpMVModel.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/Trace.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/TraceIR.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsFSC.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT1.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT2.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT3.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/SpMVGenericCode.cpp.o
spmv_lcm_d.exe: build/demo/CMakeFiles/spmv_demo.dir/build.make
spmv_lcm_d.exe:
	$(CPP)   -march=native -fopenmp -std=c++17  -march=native -O3 -DNDEBUG build/demo/CMakeFiles/spmv_demo.dir/artificial_matrix_generation.o build/demo/CMakeFiles/spmv_demo.dir/ordered_set.o build/demo/CMakeFiles/spmv_demo.dir/FusionDemo.cpp.o build/demo/CMakeFiles/spmv_demo.dir/SPMV_demo.cpp.o build/demo/CMakeFiles/spmv_demo.dir/SerialSpMVExecutor.cpp.o build/demo/CMakeFiles/spmv_demo.dir/__/src/Analyzer.cpp.o build/demo/CMakeFiles/spmv_demo.dir/__/src/DDT.cpp.o build/demo/CMakeFiles/spmv_demo.dir/__/src/DDTUtils.cpp.o build/demo/CMakeFiles/spmv_demo.dir/__/src/Executor.cpp.o build/demo/CMakeFiles/spmv_demo.dir/__/src/Input.cpp.o build/demo/CMakeFiles/spmv_demo.dir/__/src/Inspector.cpp.o build/demo/CMakeFiles/spmv_demo.dir/__/src/PatternMatching.cpp.o build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/PolyModel.cpp.o build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/SpMVModel.cpp.o build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/Trace.cpp.o build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/TraceIR.cpp.o build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsFSC.cpp.o build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT1.cpp.o build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT2.cpp.o build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/GenericCodeletsPSCT3.cpp.o build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/SpMVGenericCode.cpp.o -o spmv_lcm_d.exe  -fopenmp -lm


clean:
	rm -f build/demo/CMakeFiles/spmv_demo.dir/*.o
	rm -f build/demo/CMakeFiles/spmv_demo.dir/*.d
	rm -f build/demo/CMakeFiles/spmv_demo.dir/__/lib/aggregation/utils/*.o
	rm -f build/demo/CMakeFiles/spmv_demo.dir/__/lib/aggregation/utils/*.d
	rm -f build/demo/CMakeFiles/spmv_demo.dir/__/lib/aggregation/utils/external/*.o
	rm -f build/demo/CMakeFiles/spmv_demo.dir/__/lib/aggregation/utils/external/*.d
	rm -f build/demo/CMakeFiles/spmv_demo.dir/__/lib/*.o
	rm -f build/demo/CMakeFiles/spmv_demo.dir/__/lib/*.d
	rm -f build/demo/CMakeFiles/spmv_demo.dir/__/src/*.o
	rm -f build/demo/CMakeFiles/spmv_demo.dir/__/src/*.d
	rm -f build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/*.o
	rm -f build/demo/CMakeFiles/spmv_demo.dir/__/src/spmv_codelets/*.d
	rm -f build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/*.o
	rm -f build/demo/CMakeFiles/spmv_demo.dir/__/src/trace_gen/*.d
	rm -f *.exe
.PHONY : clean

