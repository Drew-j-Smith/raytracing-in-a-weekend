#pragma once

#include "vec3.hpp"

#include <algorithm>
#include <iostream>

static void write_color(std::ostream &out, const color &pixel_color,
                        int samples_per_pixel) {
    constexpr auto max_color = 0.999;
    constexpr auto max_color_int = 256;

    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color by the number of samples.
    auto scale = 1.0 / samples_per_pixel;
    r *= scale;
    g *= scale;
    b *= scale;

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(max_color_int * std::clamp(r, 0.0, max_color))
        << ' '
        << static_cast<int>(max_color_int * std::clamp(g, 0.0, max_color))
        << ' '
        << static_cast<int>(max_color_int * std::clamp(b, 0.0, max_color))
        << '\n';
}
