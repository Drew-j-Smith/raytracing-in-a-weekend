#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <random>

// Usings

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants

constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = 3.1415926535897932385;

// Utility Functions

[[nodiscard]] constexpr double degrees_to_radians(double degrees) {
    constexpr auto one_eighty = 180.0;
    return degrees * pi / one_eighty;
}

[[nodiscard]] inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

[[nodiscard]] inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

// Common Headers

#include "ray.hpp"
#include "vec3.hpp"
