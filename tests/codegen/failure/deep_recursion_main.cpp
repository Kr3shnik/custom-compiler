#include <iostream>

extern "C" int deep_recursion(int n);

int main() {
    std::cout << "Testing deep recursion (this might fail):" << std::endl;
    
    // This test is designed to potentially fail due to stack overflow
    // or other runtime issues with deep recursion
    int result = deep_recursion(30);  // This could cause stack overflow
    std::cout << "deep_recursion(30) = " << result << std::endl;
    
    return 0;
}
