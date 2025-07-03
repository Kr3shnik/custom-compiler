#include <iostream>

extern "C" {
    int add(int a, int b);
    int subtract(int a, int b);
    int multiply(int a, int b);
    int divide(int a, int b);
}

int main() {
    std::cout << "Testing arithmetic operations:" << std::endl;
    
    int result1 = add(10, 5);
    std::cout << "add(10, 5) = " << result1 << std::endl;
    if (result1 != 15) return 1;
    
    int result2 = subtract(10, 5);
    std::cout << "subtract(10, 5) = " << result2 << std::endl;
    if (result2 != 5) return 1;
    
    int result3 = multiply(6, 7);
    std::cout << "multiply(6, 7) = " << result3 << std::endl;
    if (result3 != 42) return 1;
    
    int result4 = divide(20, 4);
    std::cout << "divide(20, 4) = " << result4 << std::endl;
    if (result4 != 5) return 1;
    
    std::cout << "All arithmetic tests passed!" << std::endl;
    return 0;
}
