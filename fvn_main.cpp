#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <windows.h>
#include <CL/cl.h>
typedef std::chrono::high_resolution_clock Clock;

class kernel
{
    private:
        char * source_str;
        char * function_name;
        size_t source_size;
        void check_error(cl_int error);
    public:
        kernel(char * filename, char * function);
        kernel(const char * filename, const char * function);
        cl_kernel kernel_function;
        void set_arg(cl_uint arg_index, size_t arg_size, const void *arg_value);
        char * get_source_str();
        char * get_function_name();
        size_t get_source_size();
        
};
class coprocessor
{
    private:
        cl_platform_id * platform_id;
        cl_context       context;
        cl_device_id     device_id;
        cl_command_queue command_queue;
        cl_program *     program;
        cl_uint          num_programs;
        char *           device_name;
        char *           platform_name;
        void check_error(cl_int error);

    public:
        coprocessor(): platform_id(NULL), context(NULL), device_id(NULL), command_queue(NULL) {}
        ~coprocessor();
        void initialize(int platform, cl_device_type type);
        cl_mem create_buffer(cl_mem_flags flags, size_t size, void *host_ptr);
        void build_kernel(kernel * k);
        void enqueueNDRangeKernel(kernel * k, cl_uint work_dim, const size_t *global_work_offset, 
                                const size_t *global_work_size, const size_t *local_work_size, cl_uint num_events_in_wait_list,
                                const cl_event *event_wait_list, cl_event *event);
        void enqueueWriteBuffer(cl_mem buffer, cl_bool blocking_write, size_t offset, size_t cb, const void *ptr, 
                                cl_uint num_events_in_wait_list, const cl_event *event_wait_list, cl_event *event);
        void enqueueReadBuffer (cl_mem buffer, cl_bool blocking_write, size_t offset, size_t cb,       void *ptr, 
                                cl_uint num_events_in_wait_list, const cl_event *event_wait_list, cl_event *event);
        void finish();
        void pfn_notify(const char *errinfo, const void *private_info, size_t cb, void *user_data);
};

// Destructor
coprocessor::~coprocessor(void) {
    check_error(clFlush(this->command_queue));
    check_error(clFinish(this->command_queue));
    check_error(clReleaseCommandQueue(this->command_queue));
    check_error(clReleaseContext(this->context));
    free(this->platform_id);
    free(this->device_name);
    free(this->platform_name);
    free(this->program);
    //ret = clReleaseKernel(kernel);
    //ret = clReleaseProgram(program);
    //ret = clReleaseMemObject(memobj);

}
void coprocessor::initialize(int platform, cl_device_type type) {
    /* Get Platform and Device Info */
    cl_uint ret_num_platforms;
    cl_uint ret_num_devices;
    cl_int ret;
    
    std::cout << "Initializing Coprocessor..." << std::endl;
    check_error(clGetPlatformIDs(0, NULL, &ret_num_platforms));
    this->platform_id = (cl_platform_id *)malloc(ret_num_platforms * sizeof(cl_platform_id));
    check_error(clGetPlatformIDs(ret_num_platforms, this->platform_id, &ret_num_platforms));
    check_error(clGetDeviceIDs(this->platform_id[platform], type, 1, &this->device_id, &ret_num_devices));
    
    // Get platform name
    size_t platform_name_size = 1000;
    this->platform_name = (char *)malloc(sizeof(char) * platform_name_size);
    
    check_error(clGetPlatformInfo(this->platform_id[platform], CL_PLATFORM_NAME, platform_name_size, this->platform_name, &platform_name_size));
    realloc(this->platform_name, platform_name_size);

    // Get device name
    size_t device_name_size = 1000;
    this->device_name = (char *)malloc(sizeof(char)*device_name_size);

    check_error(clGetDeviceInfo(this->device_id, CL_DEVICE_NAME, device_name_size, this->device_name, &device_name_size));
    realloc(this->device_name, device_name_size);
    
    std::cout << " \t# Devices: " << ret_num_devices << std::endl;
    std::cout << " \tPlatform: " << this->platform_name << std::endl;
    std::cout << " \tDevice Name: " << this->device_name << std::endl;
    
    /* Create OpenCL context */
    // TODO: Create callback function for CreateContext argument 4. pfn_notify should be that, but it doesn't like class methods.
    this->context = clCreateContext(NULL, 1, &this->device_id,  NULL, NULL, &ret);
    check_error(ret);
    
    /* Create Command Queue */
    this->command_queue = clCreateCommandQueue(this->context, device_id, 0, &ret);
    check_error(ret);

    this->num_programs = 0;
    this->program = (cl_program *)calloc((this->num_programs), sizeof(cl_program));
}

cl_mem coprocessor::create_buffer(cl_mem_flags flags, size_t size, void *host_ptr) {
    cl_int ret;
    cl_mem buffer;
    buffer = clCreateBuffer(this->context, flags, size, host_ptr, &ret);
    check_error(ret);
    return buffer;
}

void coprocessor::check_error(cl_int error) {
 	if (error != CL_SUCCESS) {
 		std::cerr << "OpenCL call failed with error " << error << std::endl;
 		std::exit (1);
 	}
 }

void coprocessor::enqueueNDRangeKernel(kernel * k, cl_uint work_dim, const size_t *global_work_offset, 
                                const size_t *global_work_size, const size_t *local_work_size, cl_uint num_events_in_wait_list,
                                const cl_event *event_wait_list, cl_event *event)
{                                
    check_error(clEnqueueNDRangeKernel(this->command_queue, k->kernel_function, work_dim, global_work_offset, 
                                global_work_size, local_work_size, num_events_in_wait_list,
                                event_wait_list, event));
}

void coprocessor::enqueueWriteBuffer(cl_mem buffer, cl_bool blocking_write, size_t offset, size_t cb, const void *ptr, 
                                     cl_uint num_events_in_wait_list, const cl_event *event_wait_list, cl_event *event)
{
    check_error( clEnqueueWriteBuffer(this->command_queue, buffer, blocking_write, offset, cb, ptr,
                                        num_events_in_wait_list, event_wait_list, event) );
}

void coprocessor::enqueueReadBuffer (cl_mem buffer, cl_bool blocking_write, size_t offset, size_t cb, void *ptr, 
                                     cl_uint num_events_in_wait_list, const cl_event *event_wait_list, cl_event *event)
{
    check_error( clEnqueueReadBuffer(this->command_queue, buffer, blocking_write, offset, cb, ptr,
                                        num_events_in_wait_list, event_wait_list, event) );
}

void coprocessor::finish() {
    clFlush(this->command_queue);
    clFinish(this->command_queue);
}
void coprocessor::pfn_notify(const char *errinfo, const void *private_info, size_t cb, void *user_data) {
	printf("OpenCL Error (via pfn_notify): %s\n", errinfo);
}

void coprocessor::build_kernel(kernel * k){
    cl_int ret;
    
    if(!realloc(this->program, (this->num_programs + 1) * sizeof(cl_program))) {
        std::cout << "Didn't reallocate this->program" << std::endl;
        return;
    }
    
    /* Create Kernel Program from the source */
    std::cout << "Create Kernel Program from the source" << std::endl;
    char * source_str =  k->get_source_str();
    size_t source_size = k->get_source_size();
    this->program[num_programs] = clCreateProgramWithSource(this->context, 1, (const char**)&source_str, &source_size, &ret);
    check_error(ret);
    
    /* Build Kernel Program */
    std::cout << "Build Kernel Program" << std::endl;
    check_error(clBuildProgram(this->program[num_programs], 1, &this->device_id, NULL, NULL, NULL));

    /* Create OpenCL Kernel */
    std::cout << "Create OpenCL Kernel" << std::endl;
    k->kernel_function = clCreateKernel(this->program[num_programs], k->get_function_name(), &ret); 
    check_error(ret);
    this->num_programs++;
}



kernel::kernel(const char * filename, const char * function) {
    #define MAX_SOURCE_SIZE (0x100000)
    FILE *fp;
    
    /* Load the source code containing the kernel*/
    fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    
    this->source_str = (char*)malloc(MAX_SOURCE_SIZE);
    if(!this->source_str) printf("Failed to allocate kernel file memory.\n");
    this->source_size = fread(this->source_str, 1, MAX_SOURCE_SIZE, fp);
    if(!this->source_str) printf("Failed to allocate kernel file memory.\n");
    fclose(fp);
    
    this->function_name = (char *)malloc(sizeof(char)*strlen(function));
    strcpy(function_name, function);
    #undef MAX_SOURCE_SIZE
}
void kernel::set_arg(cl_uint arg_index, size_t arg_size, const void *arg_value) {
    check_error(clSetKernelArg(this->kernel_function, arg_index, arg_size, arg_value));
}
char * kernel::get_source_str() {
    return this->source_str;
}
char * kernel::get_function_name() {
    return this->function_name;
}
size_t kernel::get_source_size() {
    return this->source_size;
}
void kernel::check_error(cl_int error) {
 	if (error != CL_SUCCESS) {
 		std::cerr << "OpenCL call failed with error " << error << std::endl;
 		std::exit (1);
 	}
 }
#define FNV_PRIME	0x01000193
#define FNV(x, y)        ((x) * FNV_PRIME ^ (y))

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

int main() {
    // Create a 2GB file to search through
    size_t dag_size = 0 ;
    uint32_t prefetch_num = 4;
    uint32_t * dag = NULL;
    uint32_t * state = NULL;
    uint32_t * state_cl = NULL;
    uint32_t step_size = 512 * 1024 * 1024;
    int test_size = 1024*1024;    

    // OpenCL initialization
    coprocessor nvidia;
    nvidia.initialize(0, CL_DEVICE_TYPE_GPU);
    kernel eth_kernel("./kernels/eth.cl", "dag_search");
    nvidia.build_kernel(&eth_kernel);

    // This an algorithm to get the maximum size of memory that my system will let me have.
    do {
        dag_size = dag_size + step_size;

        dag = (uint32_t *)calloc(dag_size, sizeof(int));
        if(dag) {
            free(dag);
        } else {
            dag_size = dag_size - step_size;
            step_size = step_size / 2;
        }
        if(dag_size % 32 != 0) {
            dag_size = dag_size - (dag_size % 32);
        }
    } while (step_size != 1);
    printf("Dag size: %f MB\n", (float)(dag_size * sizeof(uint32_t))/ (1024*1024));
    // printf("Dag size multiple: %i / 32\n", (dag_size % 32));
    
    // Keep fucking asking for the memory.  Piece of shit OS.
    uint32_t failed_requests = 0;
    do{
        dag   = (uint32_t *)calloc(dag_size, sizeof(uint32_t));
        if(!dag)
            failed_requests++;
    } while(!dag && failed_requests < 1000000);
    if(dag) printf("Allocated dag memory\n");
    else {printf("Failed to allocated memory\n"); exit(0);}
    
    state = (uint32_t *)calloc(32 * test_size, sizeof(uint32_t));
    state_cl = (uint32_t *)calloc(32 * test_size, sizeof(uint32_t));
    if(state) printf("Allocated state memory\n");
    else {printf("Failed to allocated memory\n"); exit(0);}

    // Fill the file with random crap
    // printf("Fill the file with random crap\n");
    for(int i = 0; i < (dag_size); i++)
        dag[i] = std::rand();

    printf("Create %i 32 entry random state(s)\n", test_size);
    for(int i = 0; i < 32 * test_size; i++) {
        state[i] = std::rand();
        state_cl[i] = state[i];
    }
    
    printf("Beginning latency test\n");
    //for(int i = 0; i < 32; i++)
    //    std::cout << "state[" << i << "]: " << state[i] << std::endl;
    
    auto t1 = Clock::now();  
    for(int i = 0; i < test_size; i++)
        latency_test(dag, &state[32 * i], dag_size);
  	auto t2 = Clock::now();

    //for(int i = 0; i < 32; i++)
    //    std::cout << "state[" << i << "]: " << state[i] << std::endl;
    
    printf("Execution time in milliseconds = %0.3f ms\n", std::chrono::duration<double, std::milli>(t2 - t1).count());
    printf("Total Algos/sec = test_size / duration = %f\n", (float)(test_size * 1000) / std::chrono::duration<double, std::milli>(t2 - t1).count());
    
    /* OpenCL Benchmark */
    std::cout << "OpenCL Benchmark..." << std::endl;



    cl_mem m_dag, m_state;
    std::cout << "Create m_state" << std::endl;
    m_state = nvidia.create_buffer(CL_MEM_READ_WRITE, 32 * test_size * sizeof(cl_uint), NULL);
    nvidia.enqueueWriteBuffer(m_state, CL_TRUE, 0, 32 * test_size * sizeof(cl_uint), &state_cl[0], 0, NULL, NULL);
    
    std::cout << "Create m_dag" << std::endl;
    m_dag   = nvidia.create_buffer(CL_MEM_READ_ONLY, dag_size * sizeof(cl_uint), NULL);
    nvidia.enqueueWriteBuffer(m_dag, CL_TRUE, 0, dag_size * sizeof(cl_uint), &dag[0], 0, NULL, NULL);
    
    std::cout << "Setting arguments" << std::endl;
    eth_kernel.set_arg(0, sizeof(cl_mem), &m_dag);          // std::cout << "Set arg(0)" << std::endl;
    eth_kernel.set_arg(1, sizeof(cl_mem), &m_state);        // std::cout << "Set arg(1)" << std::endl;
    eth_kernel.set_arg(2, sizeof(cl_uint), &dag_size);      // std::cout << "Set arg(2)" << std::endl;
    eth_kernel.set_arg(3, sizeof(cl_uint), &prefetch_num);  // std::cout << "Set arg(3)" << std::endl;
    size_t global_work_offset = 0;
    size_t global_work_size = test_size / prefetch_num;
    size_t local_work_size = 1;
    
    for(int i = 1; i < 1024; i = i * 2) {
        prefetch_num = 1;
        global_work_size = test_size / prefetch_num;
        nvidia.enqueueWriteBuffer(m_state, CL_TRUE, 0, 32 * test_size * sizeof(cl_uint), &state_cl[0], 0, NULL, NULL);
        nvidia.enqueueWriteBuffer(m_dag, CL_TRUE, 0, dag_size * sizeof(cl_uint), &dag[0], 0, NULL, NULL);

        eth_kernel.set_arg(3, sizeof(cl_uint), &prefetch_num);  // std::cout << "Set arg(3)" << std::endl;
        
        std::cout << "enqueueNDRangeKernel..." << std::endl;
        t1 = Clock::now();  
        nvidia.enqueueNDRangeKernel(&eth_kernel, 1, &global_work_offset, &global_work_size, &local_work_size, 0, NULL, NULL);
        nvidia.finish();
        t2 = Clock::now();
        
        printf("Execution time in milliseconds = %0.3f ms\n", std::chrono::duration<double, std::milli>(t2 - t1).count());
        printf("Total Algos/sec @ prefetch %i = test_size / duration = %f\n", prefetch_num, (float)(test_size * 1000) / std::chrono::duration<double, std::milli>(t2 - t1).count());
    }
    nvidia.enqueueReadBuffer(m_state, CL_TRUE, 0, 32 * test_size * sizeof(cl_uint), &state_cl[0], 0, NULL, NULL);
    
    printf("Checking GPU answer... ");
    bool match = true;
    for(int i = 0; i < 32 * test_size; i++) {
        if(state[i] != state_cl[i]) match = false;
    }
    
    if(match) {
        std::cout << "GPU results are correct!" << std::endl;
    } else {
        std::cout << "GPU results are invalid!" << std::endl;
    }
    
    free(dag);
    free(state);
    free(state_cl);
    return 1;
}