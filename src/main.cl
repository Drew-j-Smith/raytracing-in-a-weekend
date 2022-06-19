

void kernel mult(global const ulong *a, global ulong *b) {
    int id = get_global_id(0);
    for (ulong i = 0; i < 10000000; ++i) {
        b[id] = a[id] * a[id];
    }
}