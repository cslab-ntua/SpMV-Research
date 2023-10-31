#!/bin/bash -i
#PBS -N job
#PBS -o job.out
#PBS -e job.err
#PBS -q icy
#PBS -l nodes=icy7:ppn=32
#PBS -l walltime=24:00:00

cd /home/users/dgal/Shared/benchmarks/SpMV/SpMV-Research/benchmark_code/CPU/AMD
> job.out
> job.err


cd spmv_code_bench
make clean; make -j

../run.sh

