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
    std::vector<std::string> filenames;
    filenames.push_back("./data/extincteur_obj.obj");
    filenames.push_back("./data/dchair_obj.obj");
    filenames.push_back("./data/dlamp.obj");
    filenames.push_back("./data/dtable.obj");
    filenames.push_back("./data/ducky.obj");
    filenames.push_back("./data/extincteur_obj.obj");
    filenames.push_back("./data/ladybird.obj");
    filenames.push_back("./data/mini_obj.obj");
    filenames.push_back("./data/pan_obj.obj");
    filenames.push_back("./data/pitcher.obj");
    filenames.push_back("./data/toyplane.obj");
    filenames.push_back("./data/wateringcan.obj");

    //const char * filename = "./data/extincteur_obj.obj";
    for(int i = 0; i < filenames.size(); i++){
        std::cout << filenames[i] << std::endl;
        auto t1 = Clock::now();
        obj::model extinguisher = obj::load_file(filenames[i].c_str());
        auto t2 = Clock::now();
        printf("Execution time in milliseconds = %0.3f ms\n\n", std::chrono::duration<double, std::milli>(t2 - t1).count());
    }
    return 0;
}
