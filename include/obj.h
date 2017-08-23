#ifndef __OBJ_INCLUDED__
#define __OBJ_INCLUDED__
namespace obj{
    
    // Types of data structures used in obj formats.
    struct vtx {  // Vertex
        float x;
        float y;
        float z;
    };
    
    struct vec {  //Vector
        float x;
        float y;
        float z;
    };
    
    struct tri {  // Triangle
        vtx * a;
        vtx * b;
        vtx * c;
    };
    
    struct face { // Face
        std::vector<obj::vtx *> vertices;
        std::vector<obj::vtx *> textures;
        std::vector<obj::vtx *> normals;
        std::vector<obj::tri *> triangles;
    };
    
    struct group { // Face Groups
        std::string name;
        std::vector<obj::face> faces;
    };
    
    struct model { // Model that is loaded
        std::string name;
        std::vector<obj::group> groups;
        std::vector<obj::tri *> triangles;
    };
    
    // Functions
    obj::model load_file(const char * filename);
    void triangulate(face * pface);
    void triangulate(group * pgroup);
    void triangulate(model * pmodel);
}
#endif
