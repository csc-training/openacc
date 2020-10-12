#!/bin/bash
#SBATCH --account=FIXME
#SBATCH --ntasks=1
#SBATCH --time=00:15:00
#SBATCH --gres=gpu:v100:1
#SBATCH --partition=gputest

module load pgi/19.7 cuda/10.1.168

srun ./hello

# Submit to the batch job queue with the command:
#  sbatch job.sh

# or alternatively, run directly from the command line:
#  srun --account=FIXME --ntasks=1 --time=00:15:00 --gres=gpu:v100:1 \
#       --partition=gputest ./hello
