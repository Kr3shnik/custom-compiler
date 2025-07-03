#include <iostream>

extern "C" {
    int test_if_else(int x);
    int test_while_loop(int n);
}

int main() {
    std::cout << "Testing control flow:" << std::endl;
    
    // Test if-else
    int result1 = test_if_else(5);
    std::cout << "test_if_else(5) = " << result1 << std::endl;
    if (result1 != 1) return 1;
    
    int result2 = test_if_else(-3);
    std::cout << "test_if_else(-3) = " << result2 << std::endl;
    if (result2 != 0) return 1;
    
    // Test while loop (sum of 1 to 5 = 15)
    int result3 = test_while_loop(5);
    std::cout << "test_while_loop(5) = " << result3 << std::endl;
    if (result3 != 15) return 1;
    
    std::cout << "All control flow tests passed!" << std::endl;
    return 0;
}
