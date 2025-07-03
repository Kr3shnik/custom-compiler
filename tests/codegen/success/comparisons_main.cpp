#include <iostream>

extern "C" {
    int test_comparisons(int a, int b);
    int test_less_than(int a, int b);
    int test_greater_than(int a, int b);
}

int main() {
    std::cout << "Testing comparison operations:" << std::endl;
    
    // Test equality
    int result1 = test_comparisons(5, 5);
    std::cout << "test_comparisons(5, 5) = " << result1 << std::endl;
    if (result1 != 1) return 1;
    
    int result2 = test_comparisons(5, 7);
    std::cout << "test_comparisons(5, 7) = " << result2 << std::endl;
    if (result2 != 0) return 1;
    
    // Test less than
    int result3 = test_less_than(3, 8);
    std::cout << "test_less_than(3, 8) = " << result3 << std::endl;
    if (result3 != 1) return 1;
    
    int result4 = test_less_than(8, 3);
    std::cout << "test_less_than(8, 3) = " << result4 << std::endl;
    if (result4 != 0) return 1;
    
    // Test greater than
    int result5 = test_greater_than(10, 5);
    std::cout << "test_greater_than(10, 5) = " << result5 << std::endl;
    if (result5 != 1) return 1;
    
    int result6 = test_greater_than(5, 10);
    std::cout << "test_greater_than(5, 10) = " << result6 << std::endl;
    if (result6 != 0) return 1;
    
    std::cout << "All comparison tests passed!" << std::endl;
    return 0;
}
