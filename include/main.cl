
#include "hittable.h"
#include "rand.cl"
#include "sphere.h"
#include "structs.h"

void kernel raycast(global float4 *res, uint width, struct Camera cam,
                    float4 color1, float4 color2) {
    uint height = get_global_size(0) / width;
    uint id = get_global_id(0);
    uint w_id = id % width;
    uint h_id = id / width;
    long seed = id;
    float4 curr_color = (0, 0, 0, 0);

    int iterations = 20;

    for (int j = 0; j < iterations; j++) {
        double3 direction =
            cam.m_lower_left_corner +
            cam.m_horizontal * (w_id + rand_double(&seed)) / (width - 1) +
            cam.m_vertical * (h_id + +rand_double(&seed)) / (height - 1) -
            cam.m_origin;
        double3 normalized = normalize(direction);

        struct HitRecord record;
        struct HitRecord temp_record;
        struct Ray ray = {cam.m_origin, normalized};
        double t_min = 0.001;
        double t_max = INFINITY;
        bool did_hit = false;
        int num_spheres = 2;
        struct Hittable hittables[] = {
            {.m_type = SHPERE,
             .m_material_id = 0,
             .m_sphere = {(double3)(0, 0, -1), 0.5}},
            {.m_type = SHPERE,
             .m_material_id = 0,
             .m_sphere = {(double3)(0, -100.5, -1), 100}}};
        for (int i = 0; i < num_spheres; ++i) {
            if (hit(hittables[i], ray, &temp_record, t_min, t_max)) {
                did_hit = true;
                record = temp_record;
                t_max = record.m_t;
            }
        }

        if (did_hit) {
            curr_color += 0.5F * (float4)((float)record.m_normal.x + 1,
                                          (float)record.m_normal.y + 1,
                                          (float)record.m_normal.z + 1, 1);
        } else {
            float t = 0.5 + 0.5 * normalized.y;
            curr_color += (1 - t) * color1 + t * color2;
        }
    }

    res[id] = curr_color / iterations;
}