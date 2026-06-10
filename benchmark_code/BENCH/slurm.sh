#!/bin/bash
#SBATCH --account=project_465002341
#SBATCH -p standard
#SBATCH --exclusive
#SBATCH --time 24:00:00                 # format: HH:MM:SS
#SBATCH --nodes 1                            # 1 node
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=128
#SBATCH --mem=200000                    # memory per node out of 246000MB
#SBATCH --job-name=job
#SBATCH --output=job.out
#SBATCH --error=job.err

#--account=project_465001345

# Interactive:
#     srun --account=project_465002341 --partition=standard --time=24:00:00 --nodes=1 --ntasks=1 --cpus-per-task=128 --exclusive --pty bash


cd /users/tasouioa/dgal/Shared/benchmarks/SpMV/SpMV-Research/benchmark_code/BENCH
> job.out
> job.err

module load gcc-native/14.2 2>&1

cd src
make clean; make -j

../run.sh
# ../proc_run.sh


# cd
# module load gcc/12.2.0 2>&1
# machine_info

