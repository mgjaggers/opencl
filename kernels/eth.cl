#define FNV_PRIME	0x01000193

__kernel void latency_test(__global uint * a, __global uint * b)
{
    uint result;
    uint size = 1024*1024*1024;
    uint gid = get_global_id(0);
    result = gid;
    
    for(int i = 0; i < 100; i++) {
        //printf("%i\n", result);

        result = (a[result] * FNV_PRIME ^ b[result]) % size;
    }
}