
void kernel raycast(const double3 start, global const double3 *direction,
                    global double3 *res, const double3 color1,
                    const double3 color2) {
    uint id = get_global_id(0);
    double3 normalized = normalize(direction[id]);
    double t = 0.5 + 0.5 * normalized.y;
    res[id] = (1.0 - t) * color1 + t * color2;
}