
struct ray {
    double3 origin;
    double3 direction;
};

struct hit_record {
    double3 p;
    double3 normal;
    double t;
};

struct circle {
    double3 center;
    double radius;
};

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

void kernel raycast(const double3 start, global const double3 *direction,
                    global float4 *res, const float4 color1,
                    const float4 color2) {
    uint id = get_global_id(0);
    double3 normalized = normalize(direction[id]);
    float t = 0.5 + 0.5 * normalized.y;
    res[id] = (1 - t) * color1 + t * color2;

    struct hit_record record;
    struct hit_record temp_record;
    struct ray ray = {start, normalized};
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
        res[id] = 0.5F * (float4)(record.normal.x + 1, record.normal.y + 1,
                                  record.normal.z + 1, 1);
    }
}