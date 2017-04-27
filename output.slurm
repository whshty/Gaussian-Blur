#!/bin/bash
#SBATCH --nodes=8
#SBATCH --cpus-per-task=8
#SBATCH --output=result.txt 



module load openmpi
mpicc mpi.c -o mpi -lm

cp 500.bmp 1000.bmp 1500.bmp 2000.bmp serial openmp fork mpi $PFSDIR/.
cd $PFSDIR


echo serial
echo sigma = 1 
echo 1 process
echo size 500
./serial 1 500
echo size 1000
./serial 1 1000
echo size 1500
./serial 1 1500
echo size 2000
./serial 1 2000


echo serial
echo sigma = 5 
echo 1 process
echo size 500
./serial 5 500
echo size 1000
./serial 5 1000
echo size 1500
./serial 5 1500
echo size 2000
./serial 5 2000


echo serial
echo sigma = 10
echo 1 process
echo size 500
./serial 5 500
echo size 1000
./serial 5 1000
echo size 1500
./serial 5 1500
echo size 2000
./serial 5 2000


echo -------------------

echo fork
echo sigma = 1 
echo 2 process
echo size 500
./fork 1 2 500
echo size 1000
./fork 1 2 1000
echo size 1500
./fork 1 2 1500
echo size 2000
./fork 1 2 2000


echo fork
echo sigma = 1 
echo 4 process
echo size 500
./fork 1 4 500
echo size 1000
./fork 1 4 1000
echo size 1500
./fork 1 4 1500
echo size 2000
./fork 1 4 2000



echo fork
echo sigma = 1 
echo 8 process
echo size 500
./fork 1 8 500
echo size 1000
./fork 1 8 1000
echo size 1500
./fork 1 8 1500
echo size 2000
./fork 1 8 2000



echo fork
echo sigma = 5 
echo 2 process
echo size 500
./fork 5 2 500
echo size 1000
./fork 5 2 1000
echo size 1500
./fork 5 2 1500
echo size 2000
./fork 5 2 2000


echo fork
echo sigma = 5 
echo 4 process
echo size 500
./fork 5 4 500
echo size 1000
./fork 5 4 1000
echo size 1500
./fork 5 4 1500
echo size 2000
./fork 5 4 2000



echo fork
echo sigma = 5
echo 8 process
echo size 500
./fork 5 8 500
echo size 1000
./fork 5 8 1000
echo size 1500
./fork 5 8 1500
echo size 2000
./fork 5 8 2000

echo -------------------

echo fork
echo sigma = 10 
echo 2 process
echo size 500
./fork 10 2 500
echo size 1000
./fork 10 2 1000
echo size 1500
./fork 10 2 1500
echo size 2000
./fork 10 2 2000


echo fork
echo sigma = 10 
echo 4 process
echo size 500
./fork 10 4 500
echo size 1000
./fork 10 4 1000
echo size 1500
./fork 10 4 1500
echo size 2000
./fork 10 4 2000



echo fork
echo sigma = 10 
echo 8 process
echo size 500
./fork 10 8 500
echo size 1000
./fork 10 8 1000
echo size 1500
./fork 10 8 1500
echo size 2000
./fork 10 8 2000
echo -------------------


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

echo -------------------
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




