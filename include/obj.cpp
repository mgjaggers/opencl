#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "obj.h"
namespace obj {
	obj::model load_file(const char * filename) {
        // File Variables
        std::ifstream infile;
		std::string line;
        obj::model file_model;
        
        // Line Parsing Variables
        std::string element_delimiter = " ";
        std::string index_delimiter = "/";
        std::string token;
        std::vector<std::string> line_elements;
        std::vector<std::string> face_elements; // String containing our face vertex partition
        obj::vtx * vp;
        std::vector<obj::vtx *> pvtx_array; // Vertex Position
        std::vector<obj::vtx *> tvtx_array; // Vertex Texture Coordinate
        std::vector<obj::vtx *> nvtx_array; // Vertex Normal
        std::vector<obj::face> face_array; // Face array
        std::vector<obj::group> group_array; // Object groups in the .obj file
        size_t lpos; // Line parsing position
        size_t fpos; // Face parsing position
        
        // Open .obj file here.
        infile.open(filename);
        
		if(infile.is_open()){
			while(infile.good()){
				std::getline(infile, line);
                
                // Delimit our string...
                while((lpos = line.find(element_delimiter)) != std::string::npos) {
                    token = line.substr(0, lpos);
                    line_elements.push_back(token);
                    line.erase(0, lpos + element_delimiter.length());
                }
                if(line != "") // If line isn't empty...
                    line_elements.push_back(line);
                // End of line parsing.  
                
                
                /* Obj element parsing */
                if(line_elements[0] == "") {} // Do nothing
                else if(line_elements[0] == "g") { // Group Elements
                    group_array.push_back(obj::group());
                    for(int i = 1; i < line_elements.size(); i++) {
                        if(i != line_elements.size() - 1) group_array.back().name += line_elements[i] + " ";
                        else group_array.back().name += line_elements[i];
                    }
                }
                else if(line_elements[0] == "v") { // Vertex Element
                    // Weighted vertices
                    vp = (obj::vtx *)malloc(sizeof(struct obj::vtx));
                    if(line_elements.size() == 5) {
                        if(stof(line_elements[4]) != 1.0){
                            std::cout << "Error: Use of weighted vertices not supported.  Aborting program..." << std::endl;
                            exit(1);
                        }
                    } else {
                    // Unweighted vertices
                        vp->x = stof(line_elements[1]);
                        vp->y = stof(line_elements[2]);
                        vp->z = stof(line_elements[3]);
                        pvtx_array.push_back(vp);
                    }
                }
                else if(line_elements[0] == "vt") { // Vertex Texture Element
                    vp = (obj::vtx *)malloc(sizeof(struct obj::vtx));
                    if(line_elements.size() == 4) {
                        vp->x = stof(line_elements[1]);
                        vp->y = stof(line_elements[2]);
                        vp->z = stof(line_elements[3]);
                    }
                    else if(line_elements.size() == 3) {
                        vp->x = stof(line_elements[1]);
                        vp->y = stof(line_elements[2]);
                        vp->z = (float)0.0;
                        /*tvtx_array.push_back((obj::vtx){ stof(line_elements[1]),
                                                         stof(line_elements[2]),
                                                         (float)0.0});
                                                         */
                    }
                    tvtx_array.push_back(vp);
                }
                
                else if(line_elements[0] == "vn") { // Vertex Normal Element
                    vp = (obj::vtx *)malloc(sizeof(struct obj::vtx));
                    if(line_elements.size() == 4) { 
                        vp->x = stof(line_elements[1]);
                        vp->y = stof(line_elements[2]);
                        vp->z = stof(line_elements[3]);
                        /*nvtx_array.push_back((obj::vtx){ stof(line_elements[1]),
                                                         stof(line_elements[2]),
                                                         stof(line_elements[3])});
                                                         */
                    }
                    nvtx_array.push_back(vp);
                }
                
                if(line_elements[0] == "f") { // Face Element
                    // Initialize our face element.
                    face_array.push_back(obj::face());

                    // For each vertex of the face.
                    for(int i = 1; i < line_elements.size(); i++){
                        // Parse each vertex of the '/' delimiter
                        // Delimit our string...
                        //std::cout << " " << line_elements[i];
                        while((fpos = line_elements[i].find(index_delimiter)) != std::string::npos) {
                            token = line_elements[i].substr(0, fpos);
                            //std::cout << "delimited: " << token << std::endl;
                            face_elements.push_back(token);
                            line_elements[i].erase(0, fpos + index_delimiter.length());
                        }
                        //std::cout << "delimited: " << line_elements[i] << std::endl;
                        face_elements.push_back(line_elements[i]);
                        // End of line parsing.
                        
                        
                        // Now we add the pointer of each vertex into the face.
                        if(face_elements.size() == 2) { // f v/vt
                            face_array.back().vertices.push_back(pvtx_array[stoi(face_elements[0])-1]);
                            face_array.back().textures.push_back(tvtx_array[stoi(face_elements[1])-1]);
                        } else if (face_elements.size() == 3) { // f v/vn/vt
                            face_array.back().vertices.push_back(pvtx_array[stoi(face_elements[0])-1]);
                            if(face_elements[1] != "") // f v//vt
                                face_array.back().normals.push_back(nvtx_array[stoi(face_elements[1])-1]);
                            face_array.back().textures.push_back(tvtx_array[stoi(face_elements[2])-1]);
                        }
                    face_elements.clear();
                    }
                    if(face_array.back().vertices.size() < 3) {
                        std::cout << "Warning: Deleting invalid face, vertices.size() = " << face_array.back().vertices.size() << "." << std::endl;
                        face_array.pop_back();
                    } else {
                        triangulate(&face_array.back());
                        group_array.back().faces.push_back(face_array.back());
                    }
                    //std::cout << std::endl;
                }
                
                // Clear out vector for next line.
                line_elements.clear();
			}
			infile.close();
            //Debug...
            //std::cout << "Size of Position Vertex array: Elements = " << pvtx_array.size() << " Space = "<< pvtx_array.size()*sizeof(obj::vtx)/(float)(1024*1024) << "MB" << std::endl;
            //std::cout << "Size of Texture Vertex array: " << tvtx_array.size()*sizeof(obj::vtx)/(float)(1024*1024) << "MB" << std::endl;
            std::cout << "Elements in Face array: " << face_array.size() << std::endl;
            //for(int i = 0; i < group_array.size(); i++){
            //    std::cout << "Group " << group_array[i].name << " Size: " << group_array[i].faces.size() << std::endl;
            //}
		} else {
			std::cout << "Error opening file: " << filename << std::endl;
		}
        
        file_model.name = filename;
        file_model.groups = group_array;
        //std::cout << file_model.groups.back().faces.at(1).vertices.back()->x << std::endl;
        return file_model;
	}
    
    void triangulate(face * pface) {
        obj::tri * ptri;
        
        // Two situations that I know can arise from making triangles out of polygons:
        // 1. Convex polygons
        // 2. Concave polygons.
        // 3. Saddle surfaces
        // TODO: Add in code that will test for each of these three polygons when triangulating
        
        // Triangulating for situation 1; known as the fan method.
        if(pface->vertices.size() >= 3) {
            for(int i = 0; i < pface->vertices.size() - 2; i++) {
                ptri = (obj::tri *)malloc(sizeof(struct obj::tri));
                ptri->a = pface->vertices[0 + 0];
                ptri->b = pface->vertices[1 + i];
                ptri->c = pface->vertices[2 + i];
                
                // Add triangle to face.
                pface->triangles.push_back(ptri);
            }
        }
    }
    
    void triangulate(model * pmodel) {
        
    }
}
