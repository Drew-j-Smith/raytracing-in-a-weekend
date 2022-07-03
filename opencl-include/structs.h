
#if defined __OPENCL_C_VERSION__ || defined __OPENCL_CPP_VERSION__
#define double3_impl double3
#else
#include <CL/cl2.hpp>
extern "C" {
#define double3_impl cl_double3 // NOLINT
#endif

struct Ray {
    double3_impl m_origin;
    double3_impl m_direction;
};

struct HitRecord {
    double3_impl m_p;
    double3_impl m_normal;
    double m_t;
};

struct Circle {
    double3_impl m_center;
    double m_radius;
};

struct Camera {
    double3_impl m_origin;
    double3_impl m_lower_left_corner;
    double3_impl m_horizontal;
    double3_impl m_vertical;
};

#if defined __OPENCL_C_VERSION__ || defined __OPENCL_CPP_VERSION__
#else
}
#endif