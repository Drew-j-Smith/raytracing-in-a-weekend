#pragma once

#include "vec3.hpp"

class ray {
public:
    [[nodiscard]] constexpr ray() {}
    [[nodiscard]] constexpr ray(const point3 &orig, const vec3 &dir)
        : orig(orig), dir(dir) {}

    [[nodiscard]] constexpr auto origin() const { return orig; }
    [[nodiscard]] constexpr auto direction() const { return dir; }

    [[nodiscard]] constexpr auto at(double t) const { return orig + dir * t; }

private:
    point3 orig;
    vec3 dir;
};
