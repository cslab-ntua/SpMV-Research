RM = rm -f
RMDIR = rm -rf

VPP ?= ${XILINX_VITIS}/bin/v++

all: package

package:
	-@echo $(VPP) --package --config ../../package_$(VITIS_PLATFORM).cfg ../../$(PROJECT_NAME)_system_hw_link/$(TARGET)/binary_container_1.xclbin -o binary_container_1.xclbin
	-@$(VPP) --package --config ../../package_$(VITIS_PLATFORM).cfg ../../$(PROJECT_NAME)_system_hw_link/$(TARGET)/binary_container_1.xclbin -o binary_container_1.xclbin
	cp -f binary_container_1.xclbin ../../$(PROJECT_NAME)/$(TARGET)/binary_container_1_CU$(CU).xclbin

clean:
	$(RMDIR) package.build package *.log *.package_summary binary_container_1.xclbin .Xil

.PHONY: all package clean
