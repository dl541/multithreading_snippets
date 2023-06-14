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

void atomicIncrement(std::atomic_int32_t &num)
{
    for (int i = 0; i < 10000; i++)
    {
        num += 1;
    }
}

int main()
{
    int num = 0;
    std::atomic_int32_t atomicNum = 0;
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; i++)
    {
        threads.emplace_back(std::thread(increment, std::ref(num)));
        threads.emplace_back(atomicIncrement, std::ref(atomicNum));
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    std::cout << num << "\n";
    std::cout << atomicNum << "\n";
}