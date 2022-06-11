#pragma once

#include "ray.h"

struct hit_record {
    constexpr hit_record() {}
    constexpr hit_record(bool hit) : hit(hit) {}
    constexpr hit_record(bool hit, double t) : hit(hit), t(t) {}

    bool hit{};
    point3 p{};
    vec3 normal{};
    double t{};
    bool front_face{};

    constexpr void set_face_normal(const ray &r, const vec3 &outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }

    constexpr operator bool() const { return hit; }
};

class hittable {
public:
    constexpr virtual ~hittable() = default;
    constexpr virtual hit_record hit(const ray &r, double t_min,
                                     double t_max) const = 0;
};