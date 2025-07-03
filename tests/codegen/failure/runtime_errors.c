int test_mixed_arithmetic(int x) {
    int a;
    int b;
    int c;
    
    a = x + 10;
    b = a * 2;
    c = b - 5;
    
    return c / 3;
}

int test_zero_division(int x) {
    if (x == 0) {
        return 1 / x;
    }
    return x;
}
