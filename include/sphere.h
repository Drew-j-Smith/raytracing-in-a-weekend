#include "structs.h"

#ifndef SPHERE_H
#define SPHERE_H

struct Sphere {
    double3_impl m_center;
    double m_radius;
};

bool hit_sphere(struct Sphere sphere, struct Ray ray, struct HitRecord *record,
                double t_min, double t_max) {
    double3 oc = ray.m_origin - sphere.m_center;
    double a = dot(ray.m_direction, ray.m_direction);
    double b = dot(oc, ray.m_direction);
    double c = dot(oc, oc) - sphere.m_radius * sphere.m_radius;
    double discriminant = b * b - a * c;
    if (discriminant < 0) {
        return false;
    }

    double sqrtd = sqrt(discriminant);
    double root = (-b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    record->m_t = root;
    double3 int_point = ray.m_origin + ray.m_direction * root;
    record->m_p = int_point;
    record->m_normal =
        normalize((int_point - sphere.m_center) / sphere.m_radius);

    return true;
}

#endif
