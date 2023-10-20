HOST_CXX ?= g++
RM = rm -f
RMDIR = rm -rf

EMCONFIGUTIL = ${XILINX_VITIS}/bin/emconfigutil --od .
EMCONFIG_FILE = emconfig.json
NUMBER_OF_DEVICES = 1

# host compiler global settings
CXXFLAGS += -std=c++0x -DVITIS_PLATFORM=$(VITIS_PLATFORM) -D__USE_XOPEN2K8 -I$(XILINX_XRT)/include/ -I$(XILINX_VIVADO)/include/ -I$(XILINX_VITIS_HLS)/include/ -O2 -g -Wall -c -fmessage-length=0
CXXFLAGS += -DCU=${CU} -fopenmp -I/opt/xilinx/Vivado/2020.2/include/  
LDFLAGS += -lxilinxopencl -lpthread -lrt -lstdc++ -L$(XILINX_XRT)/lib/ -Wl,-rpath-link,$(XILINX_XRT)/lib -fopenmp

HOST_OBJECTS += src2/Jac_cpt.o
HOST_OBJECTS += src2/Japply.o
HOST_OBJECTS += src2/PCG_Jac.o
HOST_OBJECTS += src2/driver_PCG_FPGA.o
HOST_OBJECTS += src2/oops.o
HOST_OBJECTS += src2/test_MxV.o
HOST_OBJECTS += src2/xcl2.o

HOST_EXE =  $(PROJECT_NAME)

BUILD_SUBDIRS += src2/

.PHONY: all clean
all:  $(HOST_EXE) $(EMCONFIG_FILE)

clean:
	-$(RM) -rf $(HOST_EXE) $(HOST_OBJECTS) $(BUILD_SUBDIRS)

.PHONY: incremental
incremental: all


nothing:

src2/Jac_cpt.o: ../src2/Jac_cpt.cpp ../src2/Jac_cpt.h
	-@mkdir -p $(@D)
	$(HOST_CXX) $(CXXFLAGS) -o "$@" "$<"

src2/Japply.o: ../src2/Japply.cpp ../src2/Japply.h
	-@mkdir -p $(@D)
	$(HOST_CXX) $(CXXFLAGS) -o "$@" "$<"

src2/PCG_Jac.o: ../src2/PCG_Jac.cpp ../src2/PCG_Jac.h ../src2/oops.hpp ../src2/Japply.h
	-@mkdir -p $(@D)
	$(HOST_CXX) $(CXXFLAGS) -o "$@" "$<"

src2/driver_PCG_FPGA.o: ../src2/driver_PCG_FPGA.cpp ../src2/oops.hpp ../src2/Jac_cpt.h ../src2/PCG_Jac.h ../src2/test_MxV.h
	-@mkdir -p $(@D)
	$(HOST_CXX) $(CXXFLAGS) -o "$@" "$<"

src2/oops.o: ../src2/oops.cpp ../src2/oops.hpp
	-@mkdir -p $(@D)
	$(HOST_CXX) $(CXXFLAGS) -o "$@" "$<"

src2/test_MxV.o: ../src2/test_MxV.cpp ../src2/test_MxV.h ../src2/oops.hpp
	-@mkdir -p $(@D)
	$(HOST_CXX) $(CXXFLAGS) -o "$@" "$<"

src2/xcl2.o: ../src2/xcl2.cpp ../src2/xcl2.hpp
	-@mkdir -p $(@D)
	$(HOST_CXX) $(CXXFLAGS) -o "$@" "$<"

$(HOST_EXE): $(HOST_OBJECTS)
	$(HOST_CXX) -o "$@" $(+) $(LDFLAGS)

$(EMCONFIG_FILE):
	$(EMCONFIGUTIL) --nd $(NUMBER_OF_DEVICES)  --platform $(VITIS_PLATFORM)
	-@$(RMDIR) TempConfig .Xil/configutil*
