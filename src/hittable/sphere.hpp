#pragma once

#include "hittable.hpp"
#include "vec3.hpp"

class sphere : public hittable {
public:
    [[nodiscard]] constexpr sphere() {}
    [[nodiscard]] constexpr sphere(point3 cen, double r)
        : center(cen), radius(r) {}

    [[nodiscard]] constexpr virtual std::optional<hit_record>
    hit(const ray &r, double t_min, double t_max) const override;

private:
    point3 center{};
    double radius{};
};

[[nodiscard]] constexpr std::optional<hit_record>
sphere::hit(const ray &r, double t_min, double t_max) const {
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
    rec.normal = (rec.p - center) / radius;
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    return std::optional<hit_record>{rec};
}
