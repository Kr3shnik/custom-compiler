int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

int min(int a, int b) {
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

int complex_calc(int x, int y, int z) {
    int maxval;
    int minval;
    maxval = max(x, y);
    minval = min(maxval, z);
    return minval + 1;
}

int test_function_calls() {
    int x;
    int y;
    x = 10;
    y = 20;
    return max(x, y) + min(x, y);
}
