#include <iostream>

extern "C" int add(int a, int b);

int main() {
    int result = add(5, 10);
    std::cout << "Result: " << result << std::endl;
    if (result == 15) {
        return 0; // Success
    } else {
        std::cerr << "Test failed: expected 15, got " << result << std::endl;
        return 1; // Failure
    }
}
