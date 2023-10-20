VPP_LINKER ?= ${XILINX_VITIS}/bin/v++
RM = rm -f
RMDIR = rm -rf

VPP_OPTS = --target sw_emu

BINARY_CONTAINERS += binary_container_1.xclbin

BUILD_SUBDIRS += binary_container_1.build
BINARY_CONTAINER_1_OBJS += ../../$(PROJECT_NAME)_kernels/$(TARGET)/build/krnl_spmv.xo

ALL_MESSAGE_FILES = $(subst .xclbin,.mdb,$(BINARY_CONTAINERS))

.PHONY: all clean
all: $(BINARY_CONTAINERS)

clean:
	-$(RM) $(BINARY_CONTAINERS) $(ALL_MESSAGE_FILES) 
	-$(RM) *.xclbin.sh *.xclbin.info *.xclbin.link_summary*
	mkdir -p $(BUILD_SUBDIRS)
	find $(BUILD_SUBDIRS) -type f -exec rm -f {} +
	-$(RMDIR) $(BUILD_SUBDIRS)
	-$(RMDIR) .Xil .ipcache *.log *.ltx

.PHONY: incremental
incremental: all

nothing:

binary_container_1.xclbin: $(BINARY_CONTAINER_1_OBJS) ../../config_CU$(CU)_$(VITIS_PLATFORM).cfg
	-@echo $(VPP_LINKER) $(VPP_OPTS) --link --config $(word 2,$^) -o"$@" $(BINARY_CONTAINER_1_OBJS) > binary_container_1.xclbin.sh
	$(VPP_LINKER) $(VPP_OPTS) --link --config $(word 2,$^) -o"$@" $(BINARY_CONTAINER_1_OBJS)
