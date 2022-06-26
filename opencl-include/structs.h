
#ifdef __OPENCL_C_VERSION__
#define double3_vec double3
#else
#include <CL/cl2.hpp>
#define double3_vec cl_double3
#endif

struct ray {
    double3_vec origin;
    double3_vec direction;
};

struct hit_record {
    double3_vec p;
    double3_vec normal;
    double t;
};

struct circle {
    double3_vec center;
    double radius;
};
