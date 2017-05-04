# Gaussian-Blur
## Parallel computing of Gaussian Blur for imaging processing 
Several different methods

* fork
* OpenMP
* MPI
* OpenACC(not finished)

## Run the code
Put the input images, code and Makefile in one directory, four size of input images are provided(500,1000,1500,2000)

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

Optional : The runtime of one single Slurm job is too long. Addtional Slurm is also provided

```
sbatch mpi.slurm
sbatch openmp.slurm
```

## MapReduce using HIPI

## Setup Environment 

Environment : Cloudera QuickStart VM

Setup HIPI

http://hipi.cs.virginia.edu/gettingstarted.html

## Run the code

Put the examples folder inside hipi

Update setting.gradle

```
include ':core', ':tools:hibImport', ... ':examples:covar', ':examples:helloWorld'
```

Create input image in your home dir and put test images in


```
tools/hibImport.sh ~/SampleImages images.hib
```

Compile the code 

```
cd examples/helloWorld
gradle jar

hadoop jar build/libs/helloWorld.jar images.hib output1

```

See the result, the result will be average time of processing images using Gaussian Blur

```
hadoop fs -cat ouput1/part-r-00000
```
