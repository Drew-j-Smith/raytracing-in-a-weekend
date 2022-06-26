
bool hit_sphere(double3 center, double radius, double3 ray, double3 origin) {
    double3 oc = origin - center;
    double a = dot(ray, ray);
    double b = 2.0 * dot(oc, ray);
    double c = dot(oc, oc) - radius * radius;
    double discriminant = b * b - 4 * a * c;
    return (discriminant > 0);
}

void kernel raycast(const double3 start, global const double3 *direction,
                    global double3 *res, const double3 color1,
                    const double3 color2) {
    uint id = get_global_id(0);
    double3 normalized = normalize(direction[id]);
    double t = 0.5 + 0.5 * normalized.y;
    res[id] = (1.0 - t) * color1 + t * color2;
    double3 sphere_center = (double3)(0, 0, -1);
    if (hit_sphere(sphere_center, 0.5, normalized, start)) {
        res[id] = (double3)(1, 0, 0);
    }
}