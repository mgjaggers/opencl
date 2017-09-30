#define FNV_PRIME	0x01000193

__kernel void latency_test(__global uint * a, __global uint * b)
{
    uint result;
    uint size = 1024*1024*256;
    uint gid = get_global_id(0);
    result = gid;
    
    for(int i = 0; i < 100; i++) {
        //printf("%i\n", result);

        result = (a[result] * FNV_PRIME ^ b[result]) % size;
    }
}
/*
void latency_test(uint32_t * dag, uint32_t * state, uint32_t size)
{
    int result[32];
    
    uint32_t init0 = state[0];
    uint32_t p;
    for(int i = 0; i < 64; i++) {
        p = (uint32_t)FNV(i ^ init0, state[i & 31]) % (size / 32);
        //printf("p: %i\n", p);
        for(int j = 0; j < 32; j++) 
            state[j] = FNV(state[j], dag[p * 32 + j]);
    }
    
}
*/

#define FNV(x, y)        ((x) * FNV_PRIME ^ (y))

__kernel void dag_search(__global uint * dag, __global uint * state, uint dag_size, uint prefetch_num)
{

    uint gid = get_global_id(0);
    uint state_idx[16];
    uint p[16];
    uint init[16];
    uint dag_mod = dag_size / 32;

    for(int i = 0; i < prefetch_num; i++) {
        state_idx[i] = gid * 32 * prefetch_num + 32 * i;
        init[i]      = state[state_idx[i]];
    }
    
    for(int i = 0; i < 64; i++) {

        for(int pi = 0; pi < prefetch_num; pi++) {
            p[pi] = FNV(i ^ init[pi], state[state_idx[pi] + (i & 31)]) % dag_mod;
            prefetch(&dag[p[pi] * 32], 32);
        }
        
        
        for(int j = 0; j < 32; j++)
            for(int pi = 0; pi < prefetch_num; pi++)
                state[state_idx[pi] + j] = FNV(state[state_idx[pi] + j], dag[p[pi] * 32 + j]);
            
    }

}
