#include <iostream>

extern "C" {
    int max(int a, int b);
    int min(int a, int b);
    int complex_calc(int x, int y, int z);
}

int main() {
    std::cout << "Testing function calls:" << std::endl;
    
    // Test max function
    int result1 = max(10, 15);
    std::cout << "max(10, 15) = " << result1 << std::endl;
    if (result1 != 15) return 1;
    
    // Test min function
    int result2 = min(10, 15);
    std::cout << "min(10, 15) = " << result2 << std::endl;
    if (result2 != 10) return 1;
    
    // Test complex calculation
    int result3 = complex_calc(20, 30, 25);
    std::cout << "complex_calc(20, 30, 25) = " << result3 << std::endl;
    if (result3 != 26) return 1;  // max(20,30)=30, min(30,25)=25, 25+1=26
    
    std::cout << "All function call tests passed!" << std::endl;
    return 0;
}
