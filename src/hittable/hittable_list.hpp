#pragma once

#include "hittable.hpp"
#include "material/material.hpp"

#include <memory>
#include <vector>

using std::make_unique;
using std::unique_ptr;
using std::vector;

class hittable_list final : public hittable {
public:
    [[nodiscard]] constexpr hittable_list() {}
    [[nodiscard]] constexpr hittable_list(
        vector<unique_ptr<hittable>> hittables)
        : objects(std::move(hittables)) {}

    [[nodiscard]] constexpr virtual const material &
    hit(const ray &r, double t_min, double t_max, hit_record &rec) const;

private:
    lambertian default_mat{color()};
    vector<unique_ptr<hittable>> objects{};
};

[[nodiscard]] constexpr const material &
hittable_list::hit(const ray &r, double t_min, double t_max,
                   hit_record &rec) const {
    rec.didHit = false;
    double clostest_t = t_max;
    hit_record temp_rec;
    const material *mat = &default_mat;

    for (const auto &object : objects) {
        if (const auto &curr_mat = object->hit(r, t_min, clostest_t, temp_rec);
            temp_rec.didHit) {
            rec = temp_rec;
            clostest_t = temp_rec.t;
            mat = &curr_mat;
        }
    }
    return *mat;
}
