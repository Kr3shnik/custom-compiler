int helper_func(int x) {
    return x * 2;
}

int another_helper(int a, int b) {
    return a + b;
}

int test_nested_calls(int x) {
    int temp;
    temp = helper_func(x);
    return another_helper(temp, 5);
}

int factorial(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}
