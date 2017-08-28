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
    Ret SCALE(T * V, U * S){
        Ret result = {.x = V->x * S[0], \
                      .y = V->y * S[0], \
                      .z = V->z * S[0]};
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
    
	void camera::generate_rays(){
		obj::vec orthoginal_dir = CROSS<obj::vec>(&this->dir, &this->up);
		obj::vtx pixel_pos = {0, 0, 0};
		obj::vec ray_vector; // ray_vector = pixel_pos - this->pos;
		rt::ray * pr; // pr = ray_vector / ||ray_vector||
		float y_scale = this->pixelx / this->pixely;
		float x_translate = tan(3.14159/180*(this->pov/2));
		
		// Setup our first pixel position
		pixel_pos = ADD<obj::vtx>(&this->dir, &pixel_pos);
		
		obj::vec up_scaled = SCALE<obj::vec>(&this->up, &y_scale);
		pixel_pos = ADD<obj::vtx>(&up_scaled, &pixel_pos);
		
		obj::vec orth_scaled = SCALE<obj::vec>(&orthoginal_dir, &x_translate);
		pixel_pos = ADD<obj::vtx>(&orth_scaled, &pixel_pos);
		std::cout << "X: " << pixel_pos.x << " Y: " << pixel_pos.y << " Z: " << pixel_pos.z << std::endl;
		for(int current_x_pixel = 0; current_x_pixel < this->pixelx; current_x_pixel++){
			for(int current_y_pixel = 0; current_y_pixel < this->pixely; current_y_pixel++){
				//pixel_pos.x = orthoginal_dir.x
				//pixel_pos.y = 
				//pixel_pos.z = 
			}
		}
	}
}
