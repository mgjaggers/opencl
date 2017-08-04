# OpenCL C Kernel Code
## http://downloads.ti.com/mctools/esd/docs/opencl/execution/kernels-workgroups-workitems.html

The code in an OpenCL C kernel represents the algorithm to be applied to a single work-item. The granularity of a work item is determined by the implementer. If we take an element wise vector add example, where we take two 1 dimensional vectors as input, add them together element wise and write the result back into the first vector, we can express a kernel to achieve this behavior with either of the following
```cpp
kernel vectorAdd(global int* A, global const int * B)
{
    int gid = get_global_id(0);
    A[gid] += B[gid];
}
```
Next example
```cpp
#define ITER 16

kernel vectorAdd(global int* A, global const int * B)
{
    int gid = get_global_id(0) * ITER;
    int i;
    for (i = 0; i < ITER; ++i)
    {
        A[gid + i] += B[gid+ i];
    }
}
```
The first kernel will perform an add of one element of the input arrays per work-item and for arrays of length 1024, the enqueueNDRangeKernel call would need to specify 1024 as the global size. The second kernel will perform sixteen element adds per work-item and for the same 1024 length input arrays, the enqueueNDRangeKernel call would only need to specify a global size of 64, the 1024 elements / 16 elements per work-item.