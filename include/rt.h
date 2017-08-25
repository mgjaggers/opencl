#ifndef __RT_INCLUDED__
#define __RT_INCLUDED__
namespace rt {
    class camera {
        private:
            obj::vec pos;
            obj::vec dir;
            float pov;
            int rpp;
            int pixelx;
            int pixely;
    };
}
#endif
