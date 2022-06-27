
#include "opencl-include/structs.h"

int rand_int(long *seed) {
    // java's implementation
    *seed = ((*seed) * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
    return (*seed) >> 16;
}

double rand_double(long *seed) {
    // java's implementation
    double curr = ((double)(rand_int(seed))) / INT_MAX + 1;
    return curr / 2;
}

bool hit_sphere(struct circle circle, struct ray ray, struct hit_record *record,
                double t_min, double t_max) {
    double3 oc = ray.origin - circle.center;
    double a = dot(ray.direction, ray.direction);
    double b = dot(oc, ray.direction);
    double c = dot(oc, oc) - circle.radius * circle.radius;
    double discriminant = b * b - a * c;
    if (discriminant < 0) {
        return false;
    }

    double sqrtd = sqrt(discriminant);
    double root = (-b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    record->t = root;
    double3 int_point = ray.origin + ray.direction * root;
    record->p = int_point;
    record->normal = normalize((int_point - circle.center) / circle.radius);

    return true;
}

void kernel raycast(global float4 *res, uint width, struct camera cam,
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
            cam.lower_left_corner +
            cam.horizontal * (w_id + rand_double(&seed)) / (width - 1) +
            cam.vertical * (h_id + +rand_double(&seed)) / (height - 1) -
            cam.origin;
        double3 normalized = normalize(direction);

        struct hit_record record;
        struct hit_record temp_record;
        struct ray ray = {cam.origin, normalized};
        double t_min = 0.001;
        double t_max = INFINITY;
        bool hit = false;
        int num_circles = 2;
        struct circle circles[] = {{(double3)(0, 0, -1), 0.5},
                                   {(double3)(0, -100.5, -1), 100}};
        for (int i = 0; i < num_circles; ++i) {
            if (hit_sphere(circles[i], ray, &temp_record, t_min, t_max)) {
                hit = true;
                record = temp_record;
                t_max = record.t;
            }
        }

        if (hit) {
            curr_color +=
                0.5F * (float4)(record.normal.x + 1, record.normal.y + 1,
                                record.normal.z + 1, 1);
        } else {
            float t = 0.5 + 0.5 * normalized.y;
            curr_color += (1 - t) * color1 + t * color2;
        }
    }

    res[id] = curr_color / iterations;
}