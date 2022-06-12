#pragma once

#include "rtweekend.hpp"

struct hit_record;

class material {
public:
    constexpr virtual ~material() = default;
    virtual void scatter(const ray &r_in, hit_record &rec) const = 0;
};

class lambertian final : public material {
public:
    [[nodiscard]] constexpr lambertian(const color &a) : albedo(a) {}

    virtual void scatter([[maybe_unused]] const ray &r_in,
                         hit_record &rec) const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero()) {
            scatter_direction = rec.normal;
        }

        rec.scatter = ray(rec.p, scatter_direction);
        rec.attenuation = albedo;
        rec.didScatter = true;
    }

private:
    const color albedo;
};

class metal final : public material {
public:
    [[nodiscard]] constexpr metal(const color &a, double f)
        : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual void scatter(const ray &r_in, hit_record &rec) const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        rec.scatter = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        rec.attenuation = albedo;
        rec.didScatter = dot(rec.scatter.direction(), rec.normal) > 0;
    }

private:
    const color albedo;
    const double fuzz;
};