
#ifdef __cplusplus
#include <CL/cl2.hpp>
extern "C" {
#define double3_vec cl_double3 // NOLINT
#else
#define double3_vec double3
#endif

struct Ray {
    double3_vec m_origin;
    double3_vec m_direction;
};

struct HitRecord {
    double3_vec m_p;
    double3_vec m_normal;
    double m_t;
};

struct Circle {
    double3_vec m_center;
    double m_radius;
};

struct Camera {
    double3_vec m_origin;
    double3_vec m_lower_left_corner;
    double3_vec m_horizontal;
    double3_vec m_vertical;
};

#ifdef __cplusplus
}
#endif