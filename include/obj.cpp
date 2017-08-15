#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "obj.h"
namespace obj {
	void load_file(const char * filename) {
        // File Variables
        std::ifstream infile;
		std::string line;
        
        // Line Parsing Variables
        std::string element_delimiter = " ";
        std::string index_delimiter = "/";
        std::string token;
        std::vector<std::string> line_elements;
        std::vector<std::string> face_elements; // String containing our face vertex partition
        std::vector<obj::vtx> pvtx_array; // Vertex Position
        std::vector<obj::vtx> tvtx_array; // Vertex Texture Coordinate
        std::vector<obj::vtx> nvtx_array; // Vertex Normal
        std::vector<obj::face>face_array; // Face array
        size_t lpos; // Line parsing position
        size_t fpos; // Face parsing position
        
        // Open .obj file here.
        infile.open(filename);
        
		if(infile.is_open()){
			while(infile.good()){
				std::getline(infile, line);
                
                // Delimit our string...
				//std::cout << line << std::endl;
                while((lpos = line.find(element_delimiter)) != std::string::npos) {
                    token = line.substr(0, lpos);
                    //std::cout << "delimited: " << token << std::endl;
                    line_elements.push_back(token);
                    line.erase(0, lpos + element_delimiter.length());
                }
                //std::cout << "delimited: " << line << std::endl;
                if(line != "") // If line isn't empty...
                    line_elements.push_back(line);
                
                // End of line parsing.  
                
                /* Obj element parsing */
                if(line_elements[0] == "v") { // Vertex Element
                    // Weighted vertices
                    if(line_elements.size() == 5) {
                        if(stof(line_elements[4]) != 1.0){
                            std::cout << "Error: Use of weighted vertices not supported.  Aborting program..." << std::endl;
                            exit(1);
                        }
                    } else {
                    // Unweighted vertices
                        pvtx_array.push_back((obj::vtx){ stof(line_elements[1]),
                                                         stof(line_elements[2]),
                                                         stof(line_elements[3])});
                    }
                }
                
                if(line_elements[0] == "vt") { // Vertex Texture Element
                    if(line_elements.size() == 4) 
                        tvtx_array.push_back((obj::vtx){ stof(line_elements[1]),
                                                         stof(line_elements[2]),
                                                         stof(line_elements[3])});
                    else if(line_elements.size() == 3)
                        tvtx_array.push_back((obj::vtx){ stof(line_elements[1]),
                                                         stof(line_elements[2]),
                                                         (float)0.0});
                }
                
                if(line_elements[0] == "vn") { // Vertex Normal Element
                    if(line_elements.size() == 4) 
                        nvtx_array.push_back((obj::vtx){ stof(line_elements[1]),
                                                         stof(line_elements[2]),
                                                         stof(line_elements[3])});
                }
                
                if(line_elements[0] == "f") { // Face Element
                    // Initialize our face element.
                    face_array.push_back((obj::face){});
                    std::cout << line_elements[0];
                    // For each vertex of the face.
                    for(int i = 1; i < line_elements.size(); i++){
                        // Parse each vertex of the '/' delimiter
                        // Delimit our string...
                        std::cout << " " << line_elements[i];
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
                    }
                    std::cout << std::endl;
                }
                
                // Clear out vector for next line.
                line_elements.clear();
			}
			infile.close();
            std::cout << "Size of Position Vertex array: " << pvtx_array.size()*sizeof(obj::vtx)/(float)(1024*1024) << "MB" << std::endl;
            std::cout << "Size of Texture Vertex array: " << tvtx_array.size()*sizeof(obj::vtx)/(float)(1024*1024) << "MB" << std::endl;
            std::cout << "Elements in Face array: " << face_array.size() << std::endl;
		} else {
			std::cout << "Error opening file: " << filename << std::endl;
		}
	}
}