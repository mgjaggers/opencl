#ifndef __RT_INCLUDED__
#define __RT_INCLUDED__
namespace rt {
    
    template <class Ret, class T, class U>
    Ret CROSS(T * V1, U * V0);
    
    
    struct ray {
        obj::vec dir;
        obj::vtx orig;
    };
    
    // Point light source
    struct light {
        obj::vtx orig;
    };
    
    float rti_mt(rt::ray * r, obj::tri * triangle);

    class camera {
        private:
            std::vector<std::vector<rt::ray *>> rays;
            std::vector<unsigned char> image;
            obj::vtx pos;
            obj::vec dir;
            obj::vec up;
            float pov;
            int rpp;
            int pixelx;
            int pixely;
            
        public:
            camera();
            void generate_rays();
            void clear_image();
            void move(float x, float y, float z);
            void look_at(float x, float y, float z);
            rt::ray * get_ray(int x, int y);
            int get_pixelx();
            int get_pixely();
    };
    
    class scene {
        private:
            std::vector<rt::camera> cameras;
            std::vector<obj::model> models;
            std::vector<rt::light> lights;
            //std::vector<unsigned char> image;
            // Acceleration structures?
            
            // Private functions
            
            
        public:
            scene();
            void add_camera(rt::camera Camera);
            void add_model(obj::model Model);
            void add_light(rt::light Light);
            void render();
        
    };
    
    
}
#endif
