#pragma once

#include <cmath>
#include <limits>
#include <memory>

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

// Common Headers

#include "ray.hpp"
#include "vec3.hpp"
