#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <CL/cl.h>

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
  char fileName[] = "./kernels/hello.cl";
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
  cl_ulong buffer_ptrs[12];
  cl_mem hello = NULL;
  cl_mem goodbye = NULL;
  cl_mem mem_ptr = NULL;
  cl_mem DAG_ptrs = NULL;
  hello   = clCreateBuffer(context, CL_MEM_READ_WRITE,MEM_SIZE * sizeof(char), NULL, &ret);
  goodbye = clCreateBuffer(context, CL_MEM_READ_WRITE,MEM_SIZE * sizeof(char), NULL, &ret);
  mem_ptr = clCreateBuffer(context, CL_MEM_READ_WRITE, 1 * sizeof(cl_ulong), NULL, &ret);
  DAG_ptrs= clCreateBuffer(context, CL_MEM_READ_WRITE, 2 * sizeof(cl_ulong), NULL, &ret);
  
  /* Create Kernel Program from the source */
  program = clCreateProgramWithSource(context, 1, (const char **)&source_str,
  (const size_t *)&source_size, &ret);
  
  /* Build Kernel Program */
  ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
  
  /* Create OpenCL Kernel */
  cl_kernel kernel_hello = NULL;
  cl_kernel kernel_goodbye = NULL;
  cl_kernel kernel_mem_ptr = NULL;
  cl_kernel print_both = NULL;
  kernel_hello = clCreateKernel(program, "hello", &ret);
  kernel_goodbye = clCreateKernel(program, "goodbye", &ret);
  kernel_mem_ptr = clCreateKernel(program, "mem_ptr", &ret);
  print_both = clCreateKernel(program, "hello_goodbye", &ret);
  
  /* Set OpenCL Kernel Parameters */
  ret = clSetKernelArg(kernel_hello, 0, sizeof(cl_mem), (void *)&hello);
  ret = clSetKernelArg(kernel_goodbye, 0, sizeof(cl_mem), (void *)&goodbye);
  ret = clSetKernelArg(print_both, 0, sizeof(cl_mem), (void *)&DAG_ptrs);
  cl_uint test = 2;
  ret = clSetKernelArg(print_both, 1, sizeof(cl_uint), &test);
   
  /* Execute OpenCL Kernel */
  ret = clEnqueueTask(command_queue, kernel_hello, 0, NULL,NULL);
  ret = clEnqueueTask(command_queue, kernel_goodbye, 0, NULL,NULL);

  /* Copy results from the memory buffer */
  ret = clEnqueueReadBuffer(command_queue, hello, CL_TRUE, 0,
  MEM_SIZE * sizeof(char),string, 0, NULL, NULL);
   
  /* Display Result */
  puts(string);
  
  /* Copy results from the memory buffer */
  ret = clEnqueueReadBuffer(command_queue, goodbye, CL_TRUE, 0,
  MEM_SIZE * sizeof(char),string, 0, NULL, NULL);
   
  /* Display Result */
  puts(string);
  
  // Now, I want to pass cl_mem goodbye to kernel_hello and then 
  ret = clSetKernelArg(kernel_mem_ptr, 0, sizeof(cl_mem), (void *)&hello);
  CheckError(ret);
  ret = clSetKernelArg(kernel_mem_ptr, 1, sizeof(cl_mem), (void *)&mem_ptr);
  CheckError(ret);
  ret = clEnqueueTask(command_queue, kernel_mem_ptr, 0, NULL,NULL);
  CheckError(ret);
  ret = clEnqueueCopyBuffer(command_queue, mem_ptr, DAG_ptrs, 0, 0 * sizeof(cl_ulong), 
                            sizeof(cl_ulong), 0, NULL, NULL);
  
  ret = clSetKernelArg(kernel_mem_ptr, 0, sizeof(cl_mem), (void *)&goodbye);
  CheckError(ret);
  ret = clSetKernelArg(kernel_mem_ptr, 1, sizeof(cl_mem), (void *)&mem_ptr);
  CheckError(ret);
  ret = clEnqueueTask(command_queue, kernel_mem_ptr, 0, NULL,NULL);
  CheckError(ret);
  ret = clEnqueueCopyBuffer(command_queue, mem_ptr, DAG_ptrs, 0, 1 * sizeof(cl_ulong),
                            sizeof(cl_ulong), 0, NULL, NULL);

  ret = clEnqueueReadBuffer(command_queue, mem_ptr, CL_TRUE, 0,
  1 * sizeof(cl_ulong),buffer_ptrs, 0, NULL, NULL);  
  printf("tada:%lu\n",buffer_ptrs[0]);
  
   ret = clEnqueueTask(command_queue, print_both, 0, NULL,NULL);

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
