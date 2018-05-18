#!/bin/bash

rm -r myoutput
mkdir myoutput
rm time.txt
touch time.txt
make clean
make sort2d

mpirun -np 2 sort2d Lab4_tests/input1 myoutput/out1 100 100
mpirun -np 2 sort2d Lab4_tests/input2 myoutput/out2 1000 1000
mpirun -np 2 sort2d Lab4_tests/input3 myoutput/out3 1000 1000
mpirun -np 2 sort2d sample_files/in.bin myoutput/out.bin 10 10

colordiff Lab4_tests/out1 myoutput/out1
colordiff Lab4_tests/out2 myoutput/out2
colordiff Lab4_tests/out3 myoutput/out3
colordiff sample_files/out.bin myoutput/out.bin

mpirun -np 1 sort2d Lab4_tests/input1 myoutput/out1 100 100
mpirun -np 1 sort2d Lab4_tests/input2 myoutput/out2 1000 1000
mpirun -np 1 sort2d Lab4_tests/input3 myoutput/out3 1000 1000
mpirun -np 1 sort2d sample_files/in.bin myoutput/out.bin 10 10

colordiff Lab4_tests/out1 myoutput/out1
colordiff Lab4_tests/out2 myoutput/out2
colordiff Lab4_tests/out3 myoutput/out3
colordiff sample_files/out.bin myoutput/out.bin

echo "Testing Large"

time mpirun -np 2 sort2d Large\ Test/custominp myoutput/large1_2  5000 5000 
colordiff Large\ Test/customout myoutput/large1_2
time mpirun -np 1 sort2d Large\ Test/custominp myoutput/large1_1  5000 5000 
colordiff Large\ Test/customout myoutput/large1_1

echo "Testing Large2"

time mpirun -np 2 sort2d Large\ Test/large2 myoutput/large2_1  1000000 1000000 
colordiff Large\ Test/large2_1 myoutput/large2_1
time mpirun -np 1 sort2d Large\ Test/large2 myoutput/large2_2  1000000 1000000 
colordiff Large\ Test/large2_2 myoutput/large2_2

echo "Testing HPC"

time mpirun -np 2 sort2d Large\ Test/large_input_HPC myoutput/largeout_HPC_1  1000 9000000 
colordiff Large\ Test/output_HPC myoutput/largeout_HPC_1
time mpirun -np 1 sort2d Large\ Test/large_input_HPC myoutput/largeout_HPC_2  1000 9000000 
colordiff Large\ Test/output_HPC myoutput/largeout_HPC_2