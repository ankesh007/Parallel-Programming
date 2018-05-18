#!/bin/bash

make clean
make sort2d

mpirun -np 2 sort2d 3 col/col
mpirun -np 2 sort2d 20 cold/col
mpirun -np 2 sort2d 5 test_pub/col
mpirun -np 2 sort2d 5 testcases/test0/col
mpirun -np 2 sort2d 100 testcases/test1/col
mpirun -np 2 sort2d 1000 testcases/test2/col

colordiff col/col0 col/col00
colordiff cold/col0 cold/col00
colordiff test_pub/col0 test_pub/col00
colordiff testcases/test0/col0 testcases/test0/col00
colordiff testcases/test1/col0 testcases/test1/col00
colordiff testcases/test2/col0 testcases/test2/col00
