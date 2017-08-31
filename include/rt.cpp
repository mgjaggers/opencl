#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include "obj.h"
#include "rt.h"

namespace rt {
	template <class Ret, class T, class U>
    Ret CROSS(T * V1, U * V0){
        Ret result = {.x = ((V1->y * V0->z) - (V1->z * V0->y)), \
                      .y = ((V1->z * V0->x) - (V1->x * V0->z)), \
                      .z = ((V1->x * V0->y) - (V1->y * V0->x))};
        return result;
    }
    
	template <class Ret, class T, class U>
	Ret ADD(T * V1, U * V0){
		Ret result = {.x = (V1->x + V0->x), .y = (V1->y + V0->y), .z = (V1->z + V0->z)};
		return result;
	}

    template <class Ret, class T, class U>
    Ret SUB(T * V1, U * V0){
        Ret result = {.x = (V1->x - V0->x), .y = (V1->y - V0->y), .z = (V1->z - V0->z)};
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
    
    camera::camera(){
		this->pos = {0, 0, 0};
		this->dir = {0, 0, 1};
		this->up  = {0, 1, 0};
		this->pov = 90.0;
		this->rpp = 1;
		this->pixelx = 640;
		this->pixely = 480;
	}
    rt::ray * camera::get_ray(int x, int y) {
        return this->rays[x][y];
    }
    
    void camera::move(float x, float y, float z){
        this->pos = {x, y, z};
    }
    
	void camera::generate_rays(){
		obj::vec orthoginal_dir = CROSS<obj::vec>(&this->dir, &this->up);
		obj::vtx pixel_pos = {0, 0, 0};
   		obj::vtx init_pixel_pos = {0, 0, 0};

		obj::vec ray_vector; // ray_vector = pixel_pos - this->pos;
        float inv_ray_length;
        obj::vec ray_unit_dir;
		rt::ray * pr; // pr = ray_vector / ||ray_vector||
		float y_scale = (float)this->pixely / (float)this->pixelx;
		float x_translate = tan(M_PI/180*(this->pov/2));
		
		// Setup our first pixel position
		init_pixel_pos = ADD<obj::vtx>(&this->dir, &this->pos);
		
		obj::vec up_scaled = SCALE<obj::vec>(&this->up, &y_scale);
		init_pixel_pos = ADD<obj::vtx>(&up_scaled, &init_pixel_pos);
		
		obj::vec orth_scaled = SCALE<obj::vec>(&orthoginal_dir, &x_translate);
		init_pixel_pos = ADD<obj::vtx>(&orth_scaled, &init_pixel_pos);
		//std::cout << "X: " << pixel_pos.x << " Y: " << pixel_pos.y << " Z: " << pixel_pos.z << std::endl;
		for(int current_x_pixel = 0; current_x_pixel < this->pixelx; current_x_pixel++){
            this->rays.push_back(std::vector<rt::ray *>());
			for(int current_y_pixel = 0; current_y_pixel < this->pixely; current_y_pixel++){
				pixel_pos.x = init_pixel_pos.x - up_scaled.x   * 2 * (0.5 + (float)current_y_pixel)/(float)this->pixely \
                                               - orth_scaled.x * 2 * (0.5 + (float)current_x_pixel)/(float)this->pixelx;
				pixel_pos.y = init_pixel_pos.y - up_scaled.y   * 2 * (0.5 + (float)current_y_pixel)/(float)this->pixely \
                                               - orth_scaled.y * 2 * (0.5 + (float)current_x_pixel)/(float)this->pixelx;
				pixel_pos.z = init_pixel_pos.z - up_scaled.z   * 2 * (0.5 + (float)current_y_pixel)/(float)this->pixely \
                                               - orth_scaled.z * 2 * (0.5 + (float)current_x_pixel)/(float)this->pixelx;
                //std::cout << "(" << current_x_pixel << ", " << current_y_pixel << "): " << "X: " << pixel_pos.x << " Y: " << pixel_pos.y << " Z: " << pixel_pos.z << std::endl;
                ray_vector = SUB<obj::vec>(&pixel_pos, &this->pos);
                inv_ray_length = 1/LENGTH(&ray_vector);
                ray_unit_dir = SCALE<obj::vec>(&ray_vector, &inv_ray_length);
                //std::cout << "(" << current_x_pixel << ", " << current_y_pixel << "): " << "X: " << ray_unit_dir.x << " Y: " << ray_unit_dir.y << " Z: " << ray_unit_dir.z << std::endl;
                pr = (rt::ray *)malloc(sizeof(rt::ray));
                pr->dir = ray_unit_dir;
                pr->orig = this->pos;
                this->rays[current_x_pixel].push_back(pr);
			}
		}
	}
}
