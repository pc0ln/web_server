#include <iostream>
#include <thread>

void func1 () {
    for (int i = 0; i < 10; ++i) {
        std::cout << "+\n";
    }
}

void func2() {
    for (int i = 0; i < 10; ++i) {
        std::cout << "-\n";
    }
}

int main() {
    std::thread one (func1);
    std::thread two (func2);
    return 0;
}