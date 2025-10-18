int main(void) {
    int x = 42;
    float y = 3.5e-1;
    double z = 1.;
    x += 1;
    if (x > y && z >= 1.0) {
        x = (x > (int)z) ? x : (int)z;
    }
    x++;
    ++x;
    return x;
}