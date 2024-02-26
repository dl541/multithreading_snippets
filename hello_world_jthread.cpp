#include <thread>
#include <iostream>
#include <chrono>

void printHelloWorld() {
    using namespace std::chrono_literals;
    std::cout << "wait for 3 seconds\n";
    std::this_thread::sleep_for(3000ms);
    std::cout << "Hello World" << "\n";
}

int main() {
    std::jthread t {printHelloWorld};
    std::cout << "main thread ended\n";
}