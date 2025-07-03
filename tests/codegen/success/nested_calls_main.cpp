#include <iostream>

extern "C" {
    int helper_func(int x);
    int another_helper(int a, int b);
    int test_nested_calls(int x);
    int factorial(int n);
}

int main() {
    std::cout << "Testing nested function calls:" << std::endl;
    
    // Test nested calls
    int result1 = test_nested_calls(10);
    std::cout << "test_nested_calls(10) = " << result1 << std::endl;
    if (result1 != 25) return 1;  // 10*2 + 5 = 25
    
    // Test factorial
    int result2 = factorial(5);
    std::cout << "factorial(5) = " << result2 << std::endl;
    if (result2 != 120) return 1;
    
    int result3 = factorial(3);
    std::cout << "factorial(3) = " << result3 << std::endl;
    if (result3 != 6) return 1;
    
    std::cout << "All nested call tests passed!" << std::endl;
    return 0;
}
