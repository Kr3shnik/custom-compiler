int deep_recursion(int n) {
    if (n <= 0) {
        return 1;
    } else {
        return deep_recursion(n - 1) + deep_recursion(n - 1);
    }
}
