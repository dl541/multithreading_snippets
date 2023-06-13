#include <thread>
#include <iostream>
#include <vector>
#include <semaphore>

std::binary_semaphore lock{1};

void increment(int &num)
{
    for (int i = 0; i < 10000; i++)
    {
        lock.acquire();
        num += 1;
        lock.release();
    }
}

int main()
{
    int num = 0;
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; i++)
    {
        threads.emplace_back(std::thread(increment, std::ref(num)));
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    std::cout << num << "\n";
}