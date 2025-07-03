#include <iostream>

extern "C" {
    int factorial(int n);
    int sum_up_to(int n);
}

int main() {
    std::cout << "Testing loop operations:" << std::endl;
    
    // Test factorial
    int result1 = factorial(5);
    std::cout << "factorial(5) = " << result1 << std::endl;
    if (result1 != 120) return 1;
    
    int result2 = factorial(4);
    std::cout << "factorial(4) = " << result2 << std::endl;
    if (result2 != 24) return 1;
    
    // Test sum
    int result3 = sum_up_to(5);
    std::cout << "sum_up_to(5) = " << result3 << std::endl;
    if (result3 != 15) return 1;  // 1+2+3+4+5 = 15
    
    int result4 = sum_up_to(10);
    std::cout << "sum_up_to(10) = " << result4 << std::endl;
    if (result4 != 55) return 1;  // 1+2+...+10 = 55
    
    std::cout << "All loop tests passed!" << std::endl;
    return 0;
}
