
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
            return rec.attenuation * ray_color(rec.scattered, world, depth - 1);
        }
        return color(0, 0, 0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = one_half * (unit_direction.y() + 1);
    return (1.0 - t) * color(1, 1, 1) + t * color(one_half, seven_tenths, 1);
}

hittable_list random_scene() {
    auto hittables = std::vector<unique_ptr<hittable>>{};

    auto ground_material = lambertian(color(0.5, 0.5, 0.5));
    hittables.push_back(make_unique<sphere<lambertian>>(point3(0, -1000, 0),
                                                        1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2,
                          b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    auto sphere_material = lambertian(albedo);
                    hittables.push_back(make_unique<sphere<lambertian>>(
                        center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    auto sphere_material = metal(albedo, fuzz);
                    hittables.push_back(make_unique<sphere<metal>>(
                        center, 0.2, sphere_material));
                } else {
                    // glass
                    auto sphere_material = dielectric(1.5);
                    hittables.push_back(make_unique<sphere<dielectric>>(
                        center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = dielectric(1.5);
    hittables.push_back(
        make_unique<sphere<dielectric>>(point3(0, 1, 0), 1.0, material1));

    auto material2 = lambertian(color(0.4, 0.2, 0.1));
    hittables.push_back(
        make_unique<sphere<lambertian>>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = metal(color(0.7, 0.6, 0.5), 0.0);
    hittables.push_back(
        make_unique<sphere<metal>>(point3(4, 1, 0), 1.0, material3));

    return hittable_list(std::move(hittables));
}

int main() {
    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 50;
    const int max_depth = 10;

    // World

    auto world = random_scene();

    // Camera

    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture,
               dist_to_focus);

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; j--) {
        std::fprintf(stderr, "\rScan lines remaining: %d ", j);
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
