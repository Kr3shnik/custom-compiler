#include <iostream>

extern "C" int test_variables();

int main() {
    int result = test_variables();
    std::cout << "test_variables() = " << result << std::endl;
    if (result == 30) {
        std::cout << "Variables test passed!" << std::endl;
        return 0;
    } else {
        std::cerr << "Variables test failed: expected 30, got " << result << std::endl;
        return 1;
    }
}
