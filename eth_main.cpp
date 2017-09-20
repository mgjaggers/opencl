#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <CL/cl.h>
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
int main() {
  cl_device_id device_id = NULL;
  cl_context context = NULL;
  cl_command_queue command_queue = NULL;
  cl_mem memobj = NULL;
  cl_program program = NULL;
  cl_kernel kernel = NULL;
  cl_platform_id* platform_id;
  cl_uint ret_num_devices;
  cl_uint ret_num_platforms;
  cl_int ret;
  
  char string[MEM_SIZE];
 
  FILE *fp;
  char fileName[] = "./kernels/eth.cl";
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
  ret = clGetPlatformIDs(0, NULL, &ret_num_platforms);
  platform_id = (cl_platform_id*)malloc(ret_num_platforms * sizeof(cl_platform_id));
  ret = clGetPlatformIDs(ret_num_platforms, platform_id, &ret_num_platforms);
  ret = clGetDeviceIDs(platform_id[0], CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);

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
  cl_mem hb_a = NULL;
  cl_mem hb_b = NULL;
  
  size_t argument_size = 1024*1024*1024;
  hb_a = clCreateBuffer(context, CL_MEM_READ_WRITE, argument_size * sizeof(cl_uint), NULL, &ret);
  hb_b = clCreateBuffer(context, CL_MEM_READ_WRITE, argument_size * sizeof(cl_uint), NULL, &ret);
  
  /* Create Kernel Program from the source */
  program = clCreateProgramWithSource(context, 1, (const char **)&source_str,
  (const size_t *)&source_size, &ret);
  
  /* Build Kernel Program */
  ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL); CheckError(ret);
  
  /* Create OpenCL Kernel */
  cl_kernel latency_test = NULL;
  latency_test = clCreateKernel(program, "latency_test", &ret); CheckError(ret);
  
  /* Set OpenCL Kernel Parameters */
  ret = clSetKernelArg(latency_test, 0, sizeof(cl_mem), (void *)&hb_a);
  ret = clSetKernelArg(latency_test, 1, sizeof(cl_mem), (void *)&hb_b);
   
  /* Execute OpenCL Kernel */
  size_t global_work_offset = 0;
  size_t global_work_size = argument_size;
  size_t local_work_size = 1;
  
  for(int i = 0; i <= 12; i++){
    local_work_size = 1 << i;
    std::cout << "Local Work Size: " << local_work_size << std::endl;
    auto t1 = Clock::now();
    //for(int j = 0; j < 100; j++)
        ret = clEnqueueNDRangeKernel(command_queue, latency_test, 1, &global_work_offset, &global_work_size, &local_work_size, 0, NULL, NULL); 
  
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
  	auto t2 = Clock::now();
    printf("Execution time in milliseconds = %0.3f ms\n", std::chrono::duration<double, std::milli>(t2 - t1).count());
    CheckError(ret);
  }
  printf("final\n");
  /* Finalization */
  ret = clFlush(command_queue);
  ret = clFinish(command_queue);
  ret = clReleaseKernel(kernel);
  ret = clReleaseProgram(program);
  ret = clReleaseMemObject(memobj);
  ret = clReleaseCommandQueue(command_queue);
  ret = clReleaseContext(context);
   
  free(source_str);
  
  
  return 0;
}
