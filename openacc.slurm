#!/bin/bash
#SBATCH --output=openacc.txt 
#SBATCH --partition=gpufermi
#SBATCH --gres=gpu:1
#SBATCH -c 12

cp 500.bmp *.c $PFSDIR/.
cd $PFSDIR

module load pgi
pgcc -ta=nvidia:cc20 -acc openacc.c -o openacc -lm


echo size 500 
./openacc 10 500

















