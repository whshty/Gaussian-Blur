# Gaussian-Blur
Parallel computing of Gaussian Blur for imaging processing

## Run the code
Put the input images, code and Makefile in one directory

```
make

```
The first parameter is the calcuation radius, the second is the size of input image

```
./serial 1 500 

./openmp 1 500

mpiexec -n 1 mpi 1 500
```

The first parameter is the calcuation radius, the second is the number of processes ,the third is the size of input image

```
./fork 1 1 500
```

## Submit the code to HPC
Use SFTP to upload all file to HPC, the output will be generated as result.txt

```
sbatch output.slurm
```