#!/usr/bin/bash
#sudo g++ main.cpp -o run -lOpenCL -I/opt/AMDAPPSDK-3.0/include/ -lstdc++ || { echo 'Compile Failed!'; exit 1; }
sudo g++ -O3 $1.cpp ./include/*.cpp -o run -lOpenCL -I/opt/AMDAPPSDK-3.0/include/ -std=gnu++11 || { echo 'Compile Failed!'; exit 1; }

./run
