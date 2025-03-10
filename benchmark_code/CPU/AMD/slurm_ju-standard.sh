#!/bin/bash
#SBATCH --account=project_465000712
#SBATCH -p ju-standard
#SBATCH --exclusive
#SBATCH --time 24:00:00                 # format: HH:MM:SS
#SBATCH --nodes 1                            # 1 node
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=128
#SBATCH --mem=200000                    # memory per node out of 246000MB
#SBATCH --job-name=job
#SBATCH --output=job.out
#SBATCH --error=job.err



cd /users/panastas/Shared/benchmarks/SpMV/SpMV-Research/benchmark_code/CPU/AMD
> job.out
> job.err

module load gcc/12.2.0 2>&1
# module load gcc/11.2.0 2>&1

cd spmv_code_bench
make clean; make -j
# cd ../

../run.sh
# ./proc_run.sh

# machine_info


