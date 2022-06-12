
#include "rtweekend.hpp"

#include "camera.hpp"
#include "color.hpp"
#include "hittable/hittable_list.hpp"
#include "hittable/sphere.hpp"

#include "material/material.hpp"

#include <iostream>

[[nodiscard]] color ray_color(const ray &r, const hittable &world, int depth) {
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0) {
        return color(0, 0, 0);
    }

    constexpr auto one_half = 0.5;
    constexpr auto seven_tenths = 0.7;
    constexpr auto t_min = 0.001;
    if (auto rec = world.hit(r, t_min, infinity); rec.didHit) {
        if (rec.didScatter) {
            return rec.attenuation * ray_color(rec.scatter, world, depth - 1);
        }
        return color(0, 0, 0);
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

    vector<unique_ptr<hittable>> hittables{};

    auto material_ground = lambertian(color(0.8, 0.8, 0.0));
    auto material_center = lambertian(color(0.7, 0.3, 0.3));
    auto material_left = metal(color(0.8, 0.8, 0.8), 0.3);
    auto material_right = metal(color(0.8, 0.6, 0.2), 1.0);

    hittables.push_back(make_unique<sphere<lambertian>>(
        point3(0.0, -100.5, -1.0), 100.0, material_ground));
    hittables.push_back(make_unique<sphere<lambertian>>(point3(0.0, 0.0, -1.0),
                                                        0.5, material_center));
    hittables.push_back(make_unique<sphere<metal>>(point3(-1.0, 0.0, -1.0), 0.5,
                                                   material_left));
    hittables.push_back(make_unique<sphere<metal>>(point3(1.0, 0.0, -1.0), 0.5,
                                                   material_right));
    hittable_list world(std::move(hittables));

    camera cam;

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; j--) {
        if (j % 10 == 0) {
            std::fprintf(stderr, "\rScan lines remaining: %d ", j);
        }
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
