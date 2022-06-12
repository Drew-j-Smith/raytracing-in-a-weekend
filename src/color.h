#pragma once

#include "vec3.h"

#include <iostream>

static void write_color(std::ostream &out, const color &pixel_color) {
    // Write the translated [0,255] value of each color component.
    constexpr auto max_color = 255.999;
    out << static_cast<int>(max_color * pixel_color.x()) << ' '
        << static_cast<int>(max_color * pixel_color.y()) << ' '
        << static_cast<int>(max_color * pixel_color.z()) << '\n';
}
