#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include "include/algorithms.h"
#include "include/helper.h"
#include "include/obj.h"
#include "CL/cl.h"
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

int main() {
    const char * filename = "./data/extincteur_obj.obj";
    auto t1 = Clock::now();
    obj::load_file(filename);
    auto t2 = Clock::now();
    printf("Execution time in milliseconds = %0.3f ms\n\n", std::chrono::duration<double, std::milli>(t2 - t1).count());
    
    return 0;
}
