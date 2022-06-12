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

    [[nodiscard]] constexpr virtual std::optional<hit_record>
    hit(const ray &r, double t_min, double t_max) const override;

private:
    vector<unique_ptr<hittable>> objects{};
};

[[nodiscard]] constexpr std::optional<hit_record>
hittable_list::hit(const ray &r, double t_min, double t_max) const {
    double clostest_t = t_max;
    std::optional<hit_record> current;

    for (const auto &object : objects) {
        if (auto record = object->hit(r, t_min, clostest_t)) {
            current = record;
            clostest_t = current->t;
        }
    }
    return current;
}
