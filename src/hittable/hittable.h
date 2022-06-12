#pragma once

#include "ray.h"
#include <optional>

struct hit_record {
    point3 p{};
    vec3 normal{};
    double t{};
    bool front_face{};

    constexpr void set_face_normal(const ray &r, const vec3 &outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    constexpr virtual ~hittable() = default;
    [[nodiscard]] constexpr virtual std::optional<hit_record>
    hit(const ray &r, double t_min, double t_max) const = 0;
};
