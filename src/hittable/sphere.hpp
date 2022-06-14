#pragma once

#include "hittable.hpp"
#include "material/material.hpp"
#include "vec3.hpp"

template <class material_type_t> class sphere final : public hittable {
public:
    [[nodiscard]] constexpr sphere() {}
    [[nodiscard]] sphere(point3 cen, double r, material_type_t mat)
        : center(cen), radius(r), mat(mat) {}

    [[nodiscard]] constexpr virtual const material &
    hit(const ray &r, double t_min, double t_max, hit_record &rec) const;

private:
    const point3 center{};
    const double radius{};
    const material_type_t mat{};
};

template <class material_type_t>
[[nodiscard]] constexpr const material &
sphere<material_type_t>::hit(const ray &r, double t_min, double t_max,
                             hit_record &rec) const {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0) {
        rec.didHit = false;
        return mat;
    }
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root) {
            rec.didHit = false;
            return mat;
        }
    }

    rec.didHit = true;
    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);

    return mat;
}
