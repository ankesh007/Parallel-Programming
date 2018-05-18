#!/bin/sh
 
### Set the project name, your department dc by default
#PBS -P cse
 
#PBS -l select=2:ncpus=24:mem=8gb:mpiprocs=24
 
### Specify "wallclock time" required for this job, hhh:mm:ss
#PBS -l walltime=00:05:00
 
#### Get environment variables from submitting shell
#PBS -l software=ANSYS
#PBS -l place=scatter
 
module load compiler/mpi/openmpi/3.0.1/gnu
 
# After job starts, must goto working directory.
# $PBS_O_WORKDIR is the directory from where the job is fired.
echo "==============================="
echo $PBS_JOBID
cat $PBS_NODEFILE
echo "==============================="
 
### cd to home directory
cd $PBS_O_WORKDIR
 
### Command to execute
time mpirun -n 48 ./sort2d <input> <output> 1000 9000000