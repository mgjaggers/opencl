#ifndef __OBJ_INCLUDED__
#define __OBJ_INCLUDED__
namespace obj{
	
	// Types of data structures used in obj formats.
	struct vtx {  // Vertex
		float x;
		float y;
		float z;
	};
	struct tri {  // Triangle
		obj::vtx * a;
		obj::vtx * b;
		obj::vtx * c;
	};
	struct face { // Face
		std::vector<obj::vtx *> vertices;
		std::vector<obj::vtx *> normals;
		std::vector<obj::vtx *> textures;
	};
	
	// Functions
	void load_file(const char * filename);
}
#endif
