#pragma once

#include "hittable.h"

#include <memory.h>
#include <vector>

using std::make_unique;
using std::unique_ptr;
using std::vector;

class hittable_list : public hittable {
public:
    constexpr hittable_list() {}
    constexpr hittable_list(vector<unique_ptr<hittable>> &&hittables)
        : objects(std::move(hittables)) {}

    constexpr void clear() { objects.clear(); }
    constexpr void add(unique_ptr<hittable> &&object) {
        objects.push_back(std::move(object));
    }

    constexpr virtual hit_record hit(const ray &r, double t_min,
                                     double t_max) const override;

private:
    vector<unique_ptr<hittable>> objects{};
};

constexpr hit_record hittable_list::hit(const ray &r, double t_min,
                                        double t_max) const {
    hit_record current_closest(false, t_max);

    for (const auto &object : objects) {
        if (hit_record record = object->hit(r, t_min, current_closest.t)) {
            current_closest = record;
        }
    }
    return current_closest;
}