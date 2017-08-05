#!/usr/bin/bash
sudo g++ main.cpp -o run -lOpenCL -I/opt/AMDAPPSDK-3.0/include/ -lstdc++ || { echo 'Compile Failed!'; exit 1; }
./run
rm run
