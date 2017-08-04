#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include "include/algorithms.h"
#include "include/helper.h"
#include "CL/cl.h"
#define MEM_SIZE (128)
#define MAX_SOURCE_SIZE (0x100000)

 void CheckError (cl_int error)
 {
 	if (error != CL_SUCCESS) {
 		std::cerr << "OpenCL call failed with error " << error << std::endl;
 		std::exit (1);
 	}
 }

int main(){
	
	std::vector<uint32_t> data, kernel_data;
	helper::get_data((char *)"data/xor_salsa8_sample.txt", &data);
	kernel_data = data;
	
	// Printout some of the data.
	for(int i = 0; i < 32; ++i){
		std::cout << "0x" << std::hex << data[i] << "\t";
		if(i%16==15) {std::cout << std::endl;}
	}
	
	std::cout << "xor_salsa Results" << std::endl;
	algorithms::xor_salsa8(&data[0],&data[16]);
	
	// Check to see how it went.
	for(int i = 0; i < 32; ++i){
		std::cout << i << ":\t0x" << data[i] << std::endl;
	}
	
	///////////////////////////////////////////////////////////////////////////////
	/* TODO: Clean this up. */
	cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_mem memobj = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;
	cl_platform_id platform_id = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret;
	
	char string[MEM_SIZE];
	
	FILE *fp;
	char fileName[] = "./kernels/scrypt.cl";
	char *source_str;
	size_t source_size;
	
	/* Load the source code containing the kernel*/
	fp = fopen(fileName, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);
	
	/* Get Platform and Device Info */
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 1, &device_id, &ret_num_devices);
	cl_int16 test;
	// Get device name
	size_t device_name_size = 1000;
	std::vector<char> device_name;  device_name.resize(device_name_size);
	
	ret = clGetDeviceInfo(device_id, CL_DEVICE_NAME, device_name_size, device_name.data(), &device_name_size);
	device_name.resize(device_name_size);
	
	std::cout << " \t# Devices: " << ret_num_devices << std::endl;
	std::cout << " \tDevice Name: " << device_name.data() << std::endl;
	
	/* Create OpenCL context */
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	
	/* Create Command Queue */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
	
	/* Create Memory Buffer */
	memobj = clCreateBuffer(context, CL_MEM_READ_WRITE,MEM_SIZE * sizeof(char), NULL, &ret);
	cl_mem b_buffer  = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
						sizeof(uint32_t) * data.size()/2, &kernel_data[ 0], &ret);
	cl_mem bx_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR, 
						sizeof(uint32_t) * data.size()/2, &kernel_data[16], &ret);
	
	/* Create Kernel Program from the source */
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str,
	(const size_t *)&source_size, &ret);
	CheckError(ret);
	/* Build Kernel Program */
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	CheckError(ret);
	/* Create OpenCL Kernel */
	kernel = clCreateKernel(program, "xor_salsa8", &ret);
	
	/* Set OpenCL Kernel Parameters */
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&b_buffer);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&bx_buffer);
	
	/* Execute OpenCL Kernel */
	ret = clEnqueueTask(command_queue, kernel, 0, NULL,NULL);
	
	/* Copy results from the memory buffer */
	ret = clEnqueueReadBuffer(command_queue, b_buffer, CL_TRUE, 0,
	sizeof(uint32_t) * data.size()/2,&kernel_data[0], 0, NULL, NULL);
	
	/* Display Result */
	// Check to see how it went.
	for(int i = 0; i < 32; ++i){
		std::cout << i << ":\t0x" << kernel_data[i] << std::endl;
	}
	
	/* Finalization */
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(memobj);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);
	
	free(source_str);
  
	
	///////////////////////////////////////////////////////////////////////////////
	
	
	
	
	return 0;
}
