#!/bin/bash
#SBATCH --nodes=8
#SBATCH --cpus-per-task=8
#SBATCH --output=mpi.txt 


module load openmpi
mpicc mpi.c -o mpi -lm

cp 500.bmp 1000.bmp 1500.bmp 2000.bmp mpi $PFSDIR/.
cd $PFSDIR

echo -------------------
echo mpi
echo sigma = 1 
echo 1 process
echo size 500
mpiexec -n 1 mpi 1 500
echo size 1000
mpiexec -n 1 mpi 1 1000
echo size 1500
mpiexec -n 1 mpi 1 1500
echo size 2000
mpiexec -n 1 mpi 1 2000

echo mpi
echo sigma = 1 
echo 2 process
echo size 500
mpiexec -n 2 mpi 1 500
echo size 1000
mpiexec -n 2 mpi 1 1000
echo size 1500
mpiexec -n 2 mpi 1 1500
echo size 2000
mpiexec -n 2 mpi 1 2000

echo mpi
echo sigma = 1 
echo 4 process
echo size 500
mpiexec -n 4 mpi 1 500
echo size 1000
mpiexec -n 4 mpi 1 1000
echo size 1500
mpiexec -n 4 mpi 1 1500
echo size 2000
mpiexec -n 4 mpi 1 2000


echo mpi
echo sigma = 1 
echo 8 process
echo size 500
mpiexec -n 8 mpi 1 500
echo size 1000
mpiexec -n 8 mpi 1 1000
echo size 1500
mpiexec -n 8 mpi 1 1500
echo size 2000
mpiexec -n 8 mpi 1 2000

echo -------------------
echo sigma = 5
echo 1 process
echo size 500
mpiexec -n 1 mpi 5 500
echo size 1000
mpiexec -n 1 mpi 5 1000
echo size 1500
mpiexec -n 1 mpi 5 1500
echo size 2000
mpiexec -n 1 mpi 5 2000


echo sigma = 5
echo 2 process
echo size 500
mpiexec -n 2 mpi 5 500
echo size 1000
mpiexec -n 2 mpi 5 1000
echo size 1500
mpiexec -n 2 mpi 5 1500
echo size 2000
mpiexec -n 2 mpi 5 2000


echo mpi
echo sigma = 5
echo 4 process
echo size 500
mpiexec -n 4 mpi 5 500
echo size 1000
mpiexec -n 4 mpi 5 1000
echo size 1500
mpiexec -n 4 mpi 5 1500
echo size 2000
mpiexec -n 4 mpi 5 2000


echo mpi
echo sigma = 5
echo 8 process
echo size 500
mpiexec -n 8 mpi 5 500
echo size 1000
mpiexec -n 8 mpi 5 1000
echo size 1500
mpiexec -n 8 mpi 5 1500
echo size 2000
mpiexec -n 8 mpi 5 2000


echo -------------------
echo sigma = 10
echo 1 process
echo size 500
mpiexec -n 1 mpi 10 500
echo size 1000
mpiexec -n 1 mpi 10 1000
echo size 1500
mpiexec -n 1 mpi 10 1500
echo size 2000
mpiexec -n 1 mpi 10 2000

echo sigma = 10
echo 2 process
echo size 500
mpiexec -n 2 mpi 10 500
echo size 1000
mpiexec -n 2 mpi 10 1000
echo size 1500
mpiexec -n 2 mpi 10 1500
echo size 2000
mpiexec -n 2 mpi 10 2000


echo mpi
echo sigma = 10
echo 4 process
echo size 500
mpiexec -n 4 mpi 10 500
echo size 1000
mpiexec -n 4 mpi 10 1000
echo size 1500
mpiexec -n 4 mpi 10 1500
echo size 2000
mpiexec -n 4 mpi 10 2000


echo mpi
echo sigma = 10
echo 8 process
echo size 500
mpiexec -n 8 mpi 10 500
echo size 1000
mpiexec -n 8 mpi 10 1000
echo size 1500
mpiexec -n 8 mpi 10 1500
echo size 2000
mpiexec -n 8 mpi 10 2000
