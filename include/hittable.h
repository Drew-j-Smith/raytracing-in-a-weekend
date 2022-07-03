
#include "sphere.h"
#include "structs.h"

enum HittableType { SHPERE };

struct Hittable {
    enum HittableType m_type;
    int m_material_id;
    union {
        struct Sphere m_sphere;
    };
};

bool hit(struct Hittable hittable, struct Ray ray, struct HitRecord *record,
         double t_min, double t_max) {
    switch (hittable.m_type) {
    case SHPERE:
        return hit_sphere(hittable.m_sphere, ray, record, t_min, t_max);
    default:
        return false;
    }
}
