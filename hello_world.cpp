#include <thread>
#include <iostream>

void print()
{
    std::cout << "Hello World"
              << "\n";
}

int main()
{
    std::thread thread_obj(&print);
    thread_obj.join();
}