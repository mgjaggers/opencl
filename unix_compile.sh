#!/usr/bin/bash
sudo gcc main.cpp -o run -lOpenCL -I/opt/AMDAPPSDK-3.0/include/ -lstdc++ || { echo 'Compile Failed!'; exit 1; }
./run
rm run
