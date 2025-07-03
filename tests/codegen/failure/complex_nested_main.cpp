#include <iostream>

extern "C" int complex_nested(int x);

int main() {
    std::cout << "Testing complex nested control flow:" << std::endl;
    
    int result1 = complex_nested(10);
    std::cout << "complex_nested(10) = " << result1 << std::endl;
    
    int result2 = complex_nested(-5);
    std::cout << "complex_nested(-5) = " << result2 << std::endl;
    
    int result3 = complex_nested(0);
    std::cout << "complex_nested(0) = " << result3 << std::endl;
    
    return 0;
}
