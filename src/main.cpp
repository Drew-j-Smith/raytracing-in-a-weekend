
#include "rtweekend.hpp"

#include "color.hpp"
#include "hittable/hittable_list.hpp"
#include "hittable/sphere.hpp"

#include <iostream>

[[nodiscard]] constexpr color ray_color(const ray &r, const hittable &world) {
    constexpr auto one_half = 0.5;
    constexpr auto seven_tenths = 0.7;
    if (auto rec = world.hit(r, 0, infinity)) {
        return one_half * (rec->normal + color(1, 1, 1));
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = one_half * (unit_direction.y() + 1);
    return (1.0 - t) * color(1, 1, 1) + t * color(one_half, seven_tenths, 1);
}

int main() {
    constexpr auto aspect_ratio = 16.0 / 9.0;
    constexpr int image_width = 400;
    constexpr int image_height = static_cast<int>(image_width / aspect_ratio);

    constexpr auto viewport_height = 2.0;
    constexpr auto viewport_width = aspect_ratio * viewport_height;
    constexpr auto focal_length = 1.0;

    hittable_list world;
    world.add(make_unique<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_unique<sphere>(point3(0, -100.5, -1), 100));

    auto origin = point3(0, 0, 0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner =
        origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; j--) {
        std::cerr << "\rScan lines remaining: " << j << ' ';
        for (int i = 0; i < image_width; i++) {
            auto u = double(i) / (image_width - 1);
            auto v = double(j) / (image_height - 1);
            ray r(origin,
                  lower_left_corner + u * horizontal + v * vertical - origin);
            color pixel_color = ray_color(r, world);
            write_color(std::cout, pixel_color);
        }
    }

    std::cerr << "\nDone\n";
}
