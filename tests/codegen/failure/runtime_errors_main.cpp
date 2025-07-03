#include <iostream>

extern "C" {
    int test_mixed_arithmetic(int x);
    int test_zero_division(int x);
}

int main() {
    std::cout << "Testing runtime edge cases:" << std::endl;
    
    // This should work fine
    int result1 = test_mixed_arithmetic(5);
    std::cout << "test_mixed_arithmetic(5) = " << result1 << std::endl;
    
    // This should work (doesn't trigger division by zero)
    int result2 = test_zero_division(10);
    std::cout << "test_zero_division(10) = " << result2 << std::endl;
    
    
    std::cout << "Runtime edge case tests completed." << std::endl;
    return 0;
}
