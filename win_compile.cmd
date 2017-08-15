:: Set compiler type
@ECHO OFF
SET CC=g++
SET MAIN=.\%1.cpp
SET FLAGS=-lOpenCL
SET INCLUDES="C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v3.2\include"

:: Run compilation of code...
@ECHO ON
%CC% %MAIN% .\include\*.cpp -o run -I%INCLUDES% %FLAGS%