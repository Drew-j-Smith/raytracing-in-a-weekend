

void kernel mult(global const ulong *a, global ulong *b) {
    int id = get_global_id(0);
    for (ulong i = 0; i < 1000; ++i) {
        b[id] = a[id] * a[id];
    }
}

void kernel raycast(const double3 start, global const double3 *direction,
                    global double3 *res) {
    uint id = get_global_id(0);
    double3 normalized = normalize(direction[id]);
    double t = 0.5 + 0.5 * normalized.y;
    res[id] =
        (1.0 - t) * ((double3)(1.0, 1.0, 1.0)) + t * ((double3)(0.5, 0.7, 1.0));
}