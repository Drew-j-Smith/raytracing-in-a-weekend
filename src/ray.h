#pragma once

#include "vec3.h"

class ray {
public:
    constexpr ray() {}
    constexpr ray(const point3 &orig, const vec3 &dir) : orig(orig), dir(dir) {}

    constexpr auto origin() const { return orig; }
    constexpr auto direction() const { return dir; }

    constexpr auto at(double t) const { return orig + dir * t; }

private:
    point3 orig;
    vec3 dir;
};
