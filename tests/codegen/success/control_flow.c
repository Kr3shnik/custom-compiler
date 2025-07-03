int test_if_else(int x) {
    if (x > 0) {
        return 1;
    } else {
        return 0;
    }
}

int test_while_loop(int n) {
    int sum;
    int i;
    sum = 0;
    i = 1;
    
    while (i <= n) {
        sum = sum + i;
        i = i + 1;
    }
    
    return sum;
}
