#!/bin/bash
#SBATCH --account=ehpc73
#SBATCH --qos gp_ehpc
#SBATCH --exclusive
#SBATCH --time 24:00:00                 # format: HH:MM:SS
#SBATCH --nodes 1                            # 1 node
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=112
#SBATCH --job-name=job
#SBATCH --output=job.out
#SBATCH --error=job.err


# Interactive:
#     srun --account=ehpc73 --qos gp_ehpc --time=24:00:00 --nodes=1 --ntasks=1 --cpus-per-task=112 --exclusive --pty bash


cd /home/ntua/ntua596182/Shared/benchmarks/SpMV/SpMV-Research/benchmark_code/CPU/AMD
> job.out
> job.err

module load gcc/13.2.0 2>&1
module load mkl/2023.1 2>&1
module load intel/2024.2
module load tbb

cd spmv_code_bench
make clean; make -j

../run.sh
# ../proc_run.sh


# cd
# module load gcc/13.2.0 2>&1
# module load mkl/2023.1 2>&1
# module load intel/2024.2
# module load tbb
# machine_info

