#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include "include/algorithms.h"
#include "include/helper.h"
#include "include/lodepng.h"
#include "include/obj.h"
#include "include/rt.h"
#include "CL/cl.h"
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;
struct screen {
    int width;
    int height;
};

template <class Ret, class T, class U>
Ret SUB(T * V1, U * V0){
	Ret result = {.x = (V1->x - V0->x), .y = (V1->y - V0->y), .z = (V1->z - V0->z)};
	return result;
}

template <class Ret, class T, class U>
Ret ADD(T * V1, U * V0){
	Ret result = {.x = (V1->x + V0->x), .y = (V1->y + V0->y), .z = (V1->z + V0->z)};
	return result;
}

template <class T, class U>
float DOT(T * V1, U * V0){
	float result = (V1->x * V0->x) + (V1->y * V0->y) + (V1->z * V0->z);
	return result;
}

template <class Ret, class T, class U>
Ret SCALE(T * V, U * S){
	Ret result = {.x = V->x * S[0], \
				  .y = V->y * S[0], \
				  .z = V->z * S[0]};
	return result;
}

template <class T>
float LENGTH(T * V0){
	float result = sqrt((V0->x * V0->x) + (V0->y * V0->y) + (V0->z * V0->z));
	return result;
}

template <class Ret, class T, class U>
Ret CROSS(T * V1, U * V0){
	Ret result = {.x = ((V1->y * V0->z) - (V1->z * V0->y)), \
				  .y = ((V1->z * V0->x) - (V1->x * V0->z)), \
				  .z = ((V1->x * V0->y) - (V1->y * V0->x))};
	return result;
}
template <class T>
void NORMALIZE(T * V0){
	float inv_ray_length;
	T result;
	inv_ray_length = 1/LENGTH(V0);
	if(inv_ray_length != 0) {
		result = SCALE<T>(V0, &inv_ray_length);
		V0->x = result.x;
		V0->y = result.y;
		V0->z = result.z;
	}
}


// This file will be for testing out our ray tracing algorithms
int main() {
    std::vector<obj::model> scene_models;
    std::vector<std::string> filenames;
    std::vector<obj::vtx *> vtx_list;
    obj::group * gp;
    obj::face * fp;
    obj::vtx * vp;
    obj::vtx * tvector = (obj::vtx *)malloc(sizeof(struct obj::vtx));
    bool centered = false;
    rt::camera test_camera;
    test_camera.move(300, 100, 300);
    test_camera.look_at(0, 0, 0);
    test_camera.generate_rays();
    
    //NOTE: this sample will overwrite the file or test.png without warning!
    const char* filename = "test.png";

    //generate some image
    unsigned width = 640, height = 480;
    std::vector<unsigned char> image;
    image.resize(width * height * 4);
    for(unsigned y = 0; y < height; y++)
    for(unsigned x = 0; x < width; x++)
    {
        //image[4 * width * y + 4 * x + 0] = 255 * !(x & y);
        //image[4 * width * y + 4 * x + 1] = x ^ y;
        //image[4 * width * y + 4 * x + 2] = x | y;
        //image[4 * width * y + 4 * x + 3] = 255;
        image[4 * width * y + 4 * x + 0] = 0;
        image[4 * width * y + 4 * x + 1] = 0;
        image[4 * width * y + 4 * x + 2] = 0;
        image[4 * width * y + 4 * x + 3] = 255;
    }
    
    //Encode the image
    unsigned error = lodepng::encode(filename, image, width, height);

    //if there's an error, display it
    if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
	
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    tvector->x = 0;
    tvector->y = 0;
    tvector->z = 0;
    
    // Load model to perform calculations on.
    filenames.push_back("./data/ducky.obj");
    //filenames.push_back("./data/dlamp.obj");
    scene_models.push_back(obj::load_file(filenames.back().c_str()));
    
    // Let's perform a translation on all the vertices in the model.
    //  This is to provide a framework for initially starting our ray tracing algorithms
    // Scene -> models -> groups -> faces -> pvtx/nvtx/tvtx
    std::cout << "Starting translation operation..." << std::endl;
    rt::ray test_ray = {{0,1,0},{0,0,0}};

    for(int imodel = 0; imodel < scene_models.size(); imodel++){// each model
        auto current_model = scene_models[imodel];
        for(int igroup = 0; igroup < current_model.groups.size(); igroup++){// each group in model.
            auto current_group = current_model.groups[igroup];
            for(int iface = 0; iface < current_group.faces.size(); iface++){// each face in each group.
            auto current_face = current_group.faces[iface];
                for(int ivertex = 0; ivertex < current_face.vertices.size(); ivertex++){// each vertex in each face.
                    vp = scene_models[imodel].groups[igroup].faces[iface].vertices[ivertex];

                    if(centered == false){
                        vtx_list.push_back(vp);
                        if((vp->x < 0) && (vp->x < tvector->x))
                            tvector->x = vp->x;
                            
                        if((vp->y < 0) && (vp->y < tvector->y))
                            tvector->y = vp->y;
                            
                        if((vp->z < 0) && (vp->z < tvector->z))
                            tvector->z = vp->z;
                    }
                }
            }
            //std::cout << "Group " << current_group.name << " done" << std::endl;
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
    float dx = 0.0;
    float dy = 1;
    float dz = 0;
    float ox = 0.0;
    float oy = 0.0;
    float oz = 0.0;
    float vpx, vpy, vpz;
    float closest_d = -1;
    float dot, dsq;
    float screen_x = 64;
    float screen_y = 48;
    int pixel_x = 640;
    int pixel_y = 480;
    float cylinder_radius = screen_x * 2.5 / pixel_x;
    float radius_sq = cylinder_radius * cylinder_radius;
    int hits = 0;
    
    
    std::cout << "cylinder_radius = " << cylinder_radius << std::endl;
    // Move our model...
    //for(int ivtx = 0; ivtx < vtx_list.size(); ivtx++){
    //    vtx_list[ivtx]->x = vtx_list[ivtx]->x - tvector->x;
    //    vtx_list[ivtx]->y = vtx_list[ivtx]->y - tvector->y - 20;
    //    vtx_list[ivtx]->z = vtx_list[ivtx]->z - tvector->z;
    //}
    int closest_tri = 0;
    float closest_distance = 0;
    
    float rti_hit;
    auto t1 = Clock::now();
	for(int pos_z = 0; pos_z < pixel_x; pos_z++) {
    for(int pos_y = 0; pos_y < pixel_y; pos_y++) {
		
		// Reset attributes for each pixel.
		closest_tri = -1;
		closest_distance = -1;
        image[4 * width * pos_y + 4 * pos_z + 0] = 0;
        image[4 * width * pos_y + 4 * pos_z + 1] = 0;
        image[4 * width * pos_y + 4 * pos_z + 2] = 0;
        image[4 * width * pos_y + 4 * pos_z + 3] = 255;
        
		for(int itri = 0; itri < scene_models[0].triangles.size(); itri++){
			rt::ray * pr = test_camera.get_ray(pos_z, pos_y);
			rti_hit = rt::rti_mt(pr, scene_models[0].triangles[itri]);
			if(rti_hit > 0.0) {
				if(closest_tri == -1) {
					closest_tri = itri;
					closest_distance = rti_hit;
				} else if(closest_distance > rti_hit) {
					closest_tri = itri;
					closest_distance = rti_hit;
				}
				// Find the normal;
				obj::vec e1, e2, N;
				float LdotN;
				e1 = SUB<obj::vec>(scene_models[0].triangles[closest_tri]->a, scene_models[0].triangles[closest_tri]->b);
				e2 = SUB<obj::vec>(scene_models[0].triangles[closest_tri]->a, scene_models[0].triangles[closest_tri]->c);
				N = CROSS<obj::vec>(&e1, &e2);
				NORMALIZE(&N);
				LdotN = DOT(&pr->dir, &N);
				std::cout << "LdotN " << std::abs((int)(255 * LdotN)) << std::endl;
				image[4 * width * pos_y + 4 * pos_z + 0] = std::abs((int)(255 * LdotN));
				image[4 * width * pos_y + 4 * pos_z + 1] = 0;
				image[4 * width * pos_y + 4 * pos_z + 2] = 0;
				image[4 * width * pos_y + 4 * pos_z + 3] = 255;

			}
		}
		
	}
	}
    error = lodepng::encode(filename, image, width, height);

	auto t2 = Clock::now();
    printf("Execution time in milliseconds = %0.3f ms\n\n", std::chrono::duration<double, std::milli>(t2 - t1).count());


    return 0;
}
