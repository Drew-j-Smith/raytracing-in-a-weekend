#pragma once

#include "hittable.hpp"
#include "material/material.hpp"
#include "vec3.hpp"

class sphere final : public hittable {
public:
    [[nodiscard]] constexpr sphere() {}
    [[nodiscard]] sphere(point3 cen, double r, shared_ptr<material> mat)
        : center(cen), radius(r), mat(mat) {}

    [[nodiscard]] virtual std::optional<hit_record>
    hit(const ray &r, double t_min, double t_max) const override;

private:
    const point3 center{};
    const double radius{};
    shared_ptr<material> mat{};
};

[[nodiscard]] std::optional<hit_record> sphere::hit(const ray &r, double t_min,
                                                    double t_max) const {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0) {
        return std::optional<hit_record>{};
    }
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root) {
            return std::optional<hit_record>{};
        }
    }

    hit_record rec;
    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat.get();
    return std::optional<hit_record>{rec};
}
