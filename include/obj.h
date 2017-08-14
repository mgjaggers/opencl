#ifndef __OBJ_INCLUDED__
#define __OBJ_INCLUDED__
namespace obj{
	
	// Types of data structures used in obj formats.
	struct vtx {
		float x;
		float y;
		float z;
	};
	struct tri {
		obj::vtx * a;
		obj::vtx * b;
		obj::vtx * c;
	};
	struct poly {
		obj::vtx * a;
		obj::vtx * b;
		obj::vtx * c;
		obj::vtx * d;
	};
}
#endif
