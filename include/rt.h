#ifndef __RT_INCLUDED__
#define __RT_INCLUDED__
namespace rt {
    
    template <class Ret, class T, class U>
    Ret CROSS(T * V1, U * V0);
    
    struct ray {
        obj::vec dir;
        obj::vtx orig;
    };

    class camera {
        private:
            std::vector<std::vector<rt::ray *>> rays;
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
            void move(float x, float y, float z);
            rt::ray * get_ray(int x, int y);
    };
    
    
}
#endif
