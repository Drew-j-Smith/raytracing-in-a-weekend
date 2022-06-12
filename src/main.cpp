
#include "rtweekend.hpp"

#include "camera.hpp"
#include "color.hpp"
#include "hittable/hittable_list.hpp"
#include "hittable/sphere.hpp"

#include <iostream>

[[nodiscard]] constexpr color ray_color(const ray &r, const hittable &world,
                                        int depth) {
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);

    constexpr auto one_half = 0.5;
    constexpr auto seven_tenths = 0.7;
    constexpr auto t_min = 0.001;
    if (auto rec = world.hit(r, t_min, infinity)) {
        point3 target = rec->p + rec->normal + random_in_unit_sphere();
        return one_half *
               ray_color(ray(rec->p, target - rec->p), world, depth - 1);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = one_half * (unit_direction.y() + 1);
    return (1.0 - t) * color(1, 1, 1) + t * color(one_half, seven_tenths, 1);
}

int main() {
    constexpr auto aspect_ratio = 16.0 / 9.0;
    constexpr int image_width = 400;
    constexpr int image_height = static_cast<int>(image_width / aspect_ratio);
    constexpr int samples_per_pixel = 100;
    constexpr int max_depth = 50;

    hittable_list world;
    world.add(make_unique<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_unique<sphere>(point3(0, -100.5, -1), 100));

    camera cam;

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; j--) {
        std::cerr << "\rScan lines remaining: " << j << ' ';
        for (int i = 0; i < image_width; i++) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone\n";
}
