# export BUILD=Emulation-SW; 
# export XCL_EMULATION_MODE=sw_emu
export BUILD=Hardware; 

export PROJECT=optima_spmv; 
export ITER="128"
export CHECK="0"
export CU=2

# export MAT_PATH=/mnt/sda/pbakos/matrix
export MAT_PATH=/home/users/pmpakos/vitis_oops_spmv2/matrix

matrices=(
StocF_1465.coo
# Fault_639.coo
# PFlow_742.coo
# Emilia_923.coo
# Heel_1138.coo
# Geo_1438.coo
# Hook_1498.coo
# Serena.coo
# Flan_1565.coo
# Bump_2911.coo
# Queen_4147.coo
# Utemp20m.coo
)

rep=1
for ((i=0;i<rep;i++)); do
    for mat in "${matrices[@]}"
    do
        # for thread in 1 2 6 12 24; do
        for thread in 24; do
            export OMP_NUM_THREADS=${thread}
            export GOMP_CPU_AFFINITY="0-23"
            export DEV_ID="0000:5e:00.1"
            ./${PROJECT}/${BUILD}/${PROJECT} ./${PROJECT}/${BUILD}/binary_container_1_CU${CU}.xclbin ${MAT_PATH}/${mat} 1 ${ITER} ${CHECK}
        done
    done
done
