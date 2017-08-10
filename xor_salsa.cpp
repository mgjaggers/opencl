#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include "include/algorithms.h"
#include "include/helper.h"
#include "CL/cl.h"
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

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
	//helper::get_data((char *)"./data/sample_set_large.dat", &data);
	//helper::get_data((char *)"./data/sample_set.dat", &data);
	//helper::get_data((char *)"./data/xor_salsa8_sample.txt", &data);
	for(int i = 0; i < 32*1000; i++){
		data.push_back(std::rand());
	}
	kernel_data = data;
	
	// Printout some of the data.
	for(int i = 0; i < 100*32; ++i){
		//std::cout << "0x" << std::hex << data[i] << "\t";
		//if(i%16==15) {std::cout << std::endl;}
	}
	
	std::cout << "CPU algorithm compute..." << std::endl;
	int j = data.size()/2;
	auto t1 = Clock::now();
    
	for(int k = 0; k < 1024*1024; k++){ //iterations loop
	for(int i = 0; i < data.size()/2; i += 16) {
	  algorithms::xor_salsa8(&data[i],&data[j + i]);
	  //algorithms::xor_salsa8(&data[j + i],&data[i]);
	}
	}
	auto t2 = Clock::now();
	//for(int i = 0; i < data.size(); i += 16) {
	//  algorithms::xor_salsa8(&data[j + i],&data[i]);
	//}
	std::cout << "CPU algorithm compute finished!" << std::endl;
	printf("Execution time in milliseconds = %0.3f ms\n\n", std::chrono::duration<double, std::milli>(t2 - t1).count());
	// Check to see how it went.
	//for(int i = 0; i < 32; ++i){
	//  std::cout << i << ":\t0x" << data[i] << std::endl;
	//}
	
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
	command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_PROFILING_ENABLE, &ret);

	/* Create Memory Buffer */
	memobj = clCreateBuffer(context, CL_MEM_READ_WRITE,MEM_SIZE * sizeof(char), NULL, &ret);
	
	// 64KB is the max buffer size for this device...
	//cl_mem b_buffer  = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
	//					sizeof(uint32_t) * (data.size()/2), &kernel_data[ 0], &ret);
	std::vector<cl_mem> b_buffer;
	b_buffer.push_back(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(uint32_t) * (data.size()/2), &kernel_data[ 0], &ret));
	b_buffer.push_back(clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(uint32_t) * (data.size()/2), &kernel_data[ 0], &ret));

	CheckError(ret);
	size_t buffer_size = 0;
	ret = clGetMemObjectInfo(b_buffer[1], CL_MEM_SIZE, sizeof(size_t),&buffer_size , NULL);
	CheckError(ret);
	
	
	std::cout << "Device buffer size: " 
			  << buffer_size << std::endl;
	cl_mem bx_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
						sizeof(uint32_t) * (data.size()/2), &kernel_data[data.size()/2], &ret);
	CheckError(ret); 

	
	/* Create Kernel Program from the source */
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str,
	(const size_t *)&source_size, &ret);
	CheckError(ret);

	/* Build Kernel Program */
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	CheckError(ret);
	/* Create OpenCL Kernel */
	kernel = clCreateKernel(program, "xor_salsa8", &ret);
	
	
	cl_event event0, event1;
	size_t global_item_size = (size_t)kernel_data.size()/32; // NOTE: You can accidentally access items in another buffer if this number is too large.
 	size_t local_item_size = 1;
	std::cout << "Device algorithm compute..." << std::endl;
	t1 = Clock::now();
	for(int i = 0; i < 1; i++){
		/* Set OpenCL Kernel Parameters */
		ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&b_buffer[i]);
		CheckError(ret);
		ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&bx_buffer);
		CheckError(ret);
		/* Execute OpenCL Kernel */
		//ret = clEnqueueTask(command_queue, kernel, 0, NULL,NULL);
		for(int k = 0; k < 1024*1024; k++){
			if(k == 0){
				ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, &event0);		
				CheckError(ret);
			} else if(k == 1024*1024 - 1) {
				ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, &event1);		
				CheckError(ret);
			} else {
				ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);		
				CheckError(ret);
			}
		}
	}
	
	clWaitForEvents(1 , &event1);
	t2 = Clock::now();
	std::cout << "Device algorithm compute finished!" << std::endl;
	//ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&bx_buffer);
	//ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_buffer);
	//ret = clEnqueueTask(command_queue, kernel, 0, NULL,NULL);
	cl_ulong time_start, time_end;
	double total_time;

	clGetEventProfilingInfo(event0, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
	clGetEventProfilingInfo(event1, CL_PROFILING_COMMAND_END,   sizeof(time_end),   &time_end,   NULL);
	total_time = time_end - time_start;
	printf("Execution time (device) in milliseconds = %0.3f ms\n", (total_time / 1000000.0) );
	printf("Execution time (host)   in milliseconds = %0.3f ms\n\n", std::chrono::duration<double, std::milli>(t2 - t1).count());
	
	/* Copy results from the memory buffer */
	ret = clEnqueueReadBuffer(command_queue, b_buffer[0], CL_TRUE, 0,
	sizeof(uint32_t) * data.size()/2,&kernel_data[0], 0, NULL, NULL);
	CheckError(ret);
	ret = clEnqueueReadBuffer(command_queue, bx_buffer, CL_TRUE, 0,
	sizeof(uint32_t) * data.size()/2,&kernel_data[data.size()/2], 0, NULL, NULL);
	CheckError(ret);

	/* Display Result */
	// Check to see how it went.
	/*
	std::cout << "entry" << "\t" << "kernel_data" << "\t" << "data" << std::endl;
	for(int i = 0; i < 32; ++i){
	  std::cout << i << ":\t0x" << kernel_data[i] << ",\t0x" << data[i] << std::endl;
	}
	*/
	
	std::cout << "kenel_data size = " << std::dec << kernel_data.size() << " " << data.size() << std::hex << std::endl;
	// Double check all values are the same...
	for(int i = 0; i < kernel_data.size(); ++i){
	  if(kernel_data[i] != data[i]){
		  std::cout << "Mismatch at: " << std::dec << i << std::endl;
		  std::cout << "kernel_data: \t" << kernel_data[i] << std::endl;
		  std::cout << "data: \t\t" << data[i] << std::endl;
		  break;
	  }
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
