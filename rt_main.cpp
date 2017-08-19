#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "include/algorithms.h"
#include "include/helper.h"
#include "include/obj.h"
#include "CL/cl.h"
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

// This file will be for testing out 
int main() {
    std::vector<obj::model> scene_models;
    std::vector<std::string> filenames;
    std::vector<obj::vtx *> vtx_list;
    obj::group * gp;
    obj::face * fp;
    obj::vtx * vp;
    obj::vtx * tvector = (obj::vtx *)malloc(sizeof(struct obj::vtx));
    bool centered = false;
    
    tvector->x = 0;
    tvector->y = 0;
    tvector->z = 0;
    
    
    // Load model to perform calculations on.
    filenames.push_back("./data/extincteur_obj.obj");
    scene_models.push_back(obj::load_file(filenames.back().c_str()));
    
    // Let's perform a translation on all the vertices in the model.
    //  This is to provide a framework for initially starting our ray tracing algorithms
    // Scene -> models -> groups -> faces -> pvtx/nvtx/tvtx
    std::cout << "Starting translation operation..." << std::endl;
    
    for(int imodel = 0; imodel < scene_models.size(); imodel++){// each model
        auto current_model = scene_models[imodel];
        for(int igroup = 0; igroup < current_model.groups.size(); igroup++){// each group in model.
            auto current_group = current_model.groups[igroup];
            for(int iface = 0; iface < current_group.faces.size(); iface++){// each face in each group.
            auto current_face = current_group.faces[iface];
                for(int ivertex = 0; ivertex < current_face.vertices.size(); ivertex++){// each vertex in each face.
                    vp = scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex];
                    /*
                    std::cout 
                    << "X: " << scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex]->x << std::endl 
                    << "Y: " << scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex]->y << std::endl 
                    << "Z: " << scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex]->z << std::endl;
                    */
                    /* Simple translate and scale for the model.
                    scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex]->x += 0.5;
                    scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex]->y += 0.5;
                    scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex]->z += 0.5;
                    
                    scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex]->x *= 0.5;
                    scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex]->y *= 0.5;
                    scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex]->z *= 0.5;
                    */
                    /* We're going to make sure that all our vertices are placed in a spot that is > 0. */
                    if(centered == false){
                        //if(std::find(vtx_list.begin(), vtx_list.end(), vp) != vtx_list.end()){
                            // Do nothing, it's in the list now...
                            //std::cout << "Pointer already list" << std::endl;
                        //} else {
                            // Add to vtx_list.
                            //std::cout << "Pointer " << vp << " added to list" << std::endl;
                            
                        //}
                        vtx_list.push_back(vp);
                        if((vp->x < 0) && (vp->x < tvector->x))
                            tvector->x = vp->x;
                            
                        if((vp->y < 0) && (vp->y < tvector->y))
                            tvector->y = vp->y;
                            
                        if((vp->z < 0) && (vp->z < tvector->z))
                            tvector->z = vp->z;
                    } else {
                        std::cout << "mX: " << scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex]->x << ", " << vp->x << ", ";
                        vp->x = vp->x - tvector->x;
                        std::cout << scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex]->x << std::endl;

                        std::cout << "mY: " << scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex]->y << ", " << vp->y << ", ";
                        vp->y = vp->y - tvector->y;
                        std::cout << scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex]->y << std::endl;

                        std::cout << "mZ: " << scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex]->z << ", " << vp->z << ", ";
                        vp->z = vp->z - tvector->z;
                        std::cout << scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex]->z << std::endl;
                       
                    }
                    /*
                    std::cout 
                    << "mX: " << scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex]->x << std::endl
                    << "mY: " << scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex]->y << std::endl 
                    << "mZ: " << scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex]->z << std::endl;
                    */
                }
            }
            std::cout << "Group " << current_group.name << " done" << std::endl;
        }
    }
    
    std::cout << "vtx list size: " << vtx_list.size() << std::endl;
    std::sort(vtx_list.begin(), vtx_list.end());
    std::vector<obj::vtx *>::iterator it;
    it = std::unique(vtx_list.begin(), vtx_list.end());
    vtx_list.resize(std::distance(vtx_list.begin(), it));
    std::cout << "vtx list size after sort+uniq: " << vtx_list.size() << std::endl;
    std::cout << "Starting second loop" << std::endl;
    
    // Going to do some quick vertex in cylinder stuff right here...
    float dx = 1.0;
    float dy = 0;
    float dz = 0;
    float dot, dsq;
    float screen_x = 6.4;
    float screen_y = 4.8;
    int pixel_x = 640;
    int pixel_y = 480;
    float cylinder_radius = screen_x ;
    float radius_sq = cylinder_radius * cylinder_radius;
    int hits = 0;
    auto t1 = Clock::now();
    for(int ivtx = 0; ivtx < vtx_list.size(); ivtx++){
        vtx_list[ivtx]->x = vtx_list[ivtx]->x - tvector->x;
        vtx_list[ivtx]->y = vtx_list[ivtx]->y - tvector->y;
        vtx_list[ivtx]->z = vtx_list[ivtx]->z - tvector->z;
        //std::cout << "X: " << vtx_list[ivtx]->x << " Y: " << vtx_list[ivtx]->y << " Z: " << vtx_list[ivtx]->z << std::endl;
        
        //Cylinder thing.
        dot = vtx_list[ivtx]->x * dx + vtx_list[ivtx]->y * dy + vtx_list[ivtx]->z * dz;
        if( dot < 0.0f) {
            std::cout << "X: " << vtx_list[ivtx]->x << " Y: " << vtx_list[ivtx]->y << " Z: " << vtx_list[ivtx]->z << std::endl;
            std::cout << "It's behind me?" << std::endl;
        } else {
            
            dsq = vtx_list[ivtx]->x*vtx_list[ivtx]->x + vtx_list[ivtx]->y * vtx_list[ivtx]->y + vtx_list[ivtx]->z * vtx_list[ivtx]->z;
            if(dsq > radius_sq) {
                
            } else {
                //std::cout << "Inside my cylinder?" << std::endl;
                hits++;
            }
            
        }
        
        
    }
    auto t2 = Clock::now();
    printf("Execution time in milliseconds = %0.3f ms\n\n", std::chrono::duration<double, std::milli>(t2 - t1).count());
    std::cout 
    << "tX: " << tvector->x << std::endl 
    << "tY: " << tvector->y << std::endl 
    << "tZ: " << tvector->z << std::endl
    << "Hits: " << hits << std::endl;

    return 0;
}