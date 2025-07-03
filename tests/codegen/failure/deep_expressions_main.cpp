#include <iostream>

extern "C" {
    int deeply_nested_expr();
}

int main() {
    std::cout << "Testing deeply nested expressions:" << std::endl;
    
    int result = deeply_nested_expr();
    std::cout << "deeply_nested_expr() = " << result << std::endl;
    
    // This test might fail due to division by zero or complex expression handling
    std::cout << "Deep expression test executed." << std::endl;
    return 0;
}
