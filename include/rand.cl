
int rand_int(long *seed) {
    // java's implementation
    *seed = ((*seed) * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
    return (*seed) >> 16;
}

double rand_double(long *seed) {
    double curr = ((double)(rand_int(seed))) / INT_MAX + 1;
    return curr / 2;
}