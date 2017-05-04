#!/bin/bash
#SBATCH --nodes=8
#SBATCH --cpus-per-task=8
#SBATCH --output=openmp.txt 


module load openmpi
mpicc mpi.c -o mpi -lm

cp 500.bmp 1000.bmp 1500.bmp 2000.bmp openmp $PFSDIR/.
cd $PFSDIR

echo -------------------
echo openmp
echo sigma = 1
echo 1 threads
export OMP_NUM_THREADS=1
echo size 500
./openmp 1 500
echo size 1000
./openmp 1 1000
echo size 1500
./openmp 1 1500
echo size 2000
./openmp 1 2000

echo openmp
echo sigma = 1
echo 2 threads
export OMP_NUM_THREADS=2
echo size 500
./openmp 1 500
echo size 1000
./openmp 1 1000
echo size 1500
./openmp 1 1500
echo size 2000
./openmp 1 2000

echo openmp
echo sigma = 1
echo 4 threads
export OMP_NUM_THREADS=4
echo size 500
./openmp 1 500
echo size 1000
./openmp 1 1000
echo size 1500
./openmp 1 1500
echo size 2000
./openmp 1 2000

echo openmp
echo sigma = 1
echo 8 threads
export OMP_NUM_THREADS=8
echo size 500
./openmp 1 500
echo size 1000
./openmp 1 1000
echo size 1500
./openmp 1 1500
echo size 2000
./openmp 1 2000

echo -------------------
echo openmp
echo sigma = 5
echo 1 threads
export OMP_NUM_THREADS=1
echo size 500
./openmp 5 500
echo size 1000
./openmp 5 1000
echo size 1500
./openmp 5 1500
echo size 2000
./openmp 5 2000


echo openmp
echo sigma = 5
echo 2 threads
export OMP_NUM_THREADS=2
echo size 500
./openmp 5 500
echo size 1000
./openmp 5 1000
echo size 1500
./openmp 5 1500
echo size 2000
./openmp 5 2000


echo openmp
echo sigma = 5
echo 2 threads
export OMP_NUM_THREADS=2
echo size 500
./openmp 5 500
echo size 1000
./openmp 5 1000
echo size 1500
./openmp 5 1500
echo size 2000
./openmp 5 2000

echo openmp
echo sigma = 5
echo 4 threads
export OMP_NUM_THREADS=4
echo size 500
./openmp 5 500
echo size 1000
./openmp 5 1000
echo size 1500
./openmp 5 1500
echo size 2000
./openmp 5 2000

echo openmp
echo sigma = 5
echo 8 threads
export OMP_NUM_THREADS=8
echo size 500
./openmp 5 500
echo size 1000
./openmp 5 1000
echo size 1500
./openmp 5 1500
echo size 2000
./openmp 5 2000



echo -------------------
echo openmp
echo sigma = 10
echo 1 threads
export OMP_NUM_THREADS=1
echo size 500
./openmp 10 500
echo size 1000
./openmp 10 1000
echo size 1500
./openmp 10 1500
echo size 2000
./openmp 10 2000

echo openmp
echo sigma = 10
echo 2 threads
export OMP_NUM_THREADS=2
echo size 500
./openmp 10 500
echo size 1000
./openmp 10 1000
echo size 1500
./openmp 10 1500
echo size 2000
./openmp 10 2000

echo openmp
echo sigma = 10
echo 4 threads
export OMP_NUM_THREADS=4
echo size 500
./openmp 10 500
echo size 1000
./openmp 10 1000
echo size 1500
./openmp 10 1500
echo size 2000
./openmp 10 2000

echo openmp
echo sigma = 10
echo 8 threads
export OMP_NUM_THREADS=8
echo size 500
./openmp 10 500
echo size 1000
./openmp 10 1000
echo size 1500
./openmp 10 1500
echo size 2000
./openmp 10 2000
