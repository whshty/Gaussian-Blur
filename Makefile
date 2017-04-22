all:
	gcc -o serial serial.c -lm
	gcc -o fork fork.c -lm -lrt
	gcc -o openmp openmp.c -fopenmp -lm
	mpicc mpi.c -o mpi -lm
