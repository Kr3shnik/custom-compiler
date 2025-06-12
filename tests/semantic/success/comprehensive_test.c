int factorial(int n) {
    if (n == 1) {
        return 1;
    } else {
        /* return n * factorial(n - 1); */
        return 0;
    }
    return 0;
}

void demonstrateTypes(){
    int x;
    float y;
    x = 10;
    y = 3.14;
    y = x;
    x = y;
}

void main(){
    int result;
    int x;
    float pi;
    
    x = 5;
    pi = 3.14;
    result = factorial(x);
    print(result);
    print(x);
    print(pi);
    
    demonstrateTypes();
}
