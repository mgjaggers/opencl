#!/usr/bin/bash
#sudo g++ main.cpp -o run -lOpenCL -I/opt/AMDAPPSDK-3.0/include/ -lstdc++ || { echo 'Compile Failed!'; exit 1; }
/opt/intel/bin/icc xor_salsa.cpp ./include/*.cpp -o run -opencl -I/opt/AMDAPPSDK-3.0/include/ -std=gnu++11 || { echo 'Compile Failed!'; exit 1; }

./run
