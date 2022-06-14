#pragma once

#include "ray.hpp"

class material;

struct hit_record {
    point3 p{};
    vec3 normal{};
    double t{};
    bool front_face{};
    bool didHit{};

    constexpr void set_face_normal(const ray &r, const vec3 &outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    constexpr virtual ~hittable() = default;
    [[nodiscard]] constexpr virtual const material &
    hit(const ray &r, double t_min, double t_max, hit_record &rec) const = 0;
};
