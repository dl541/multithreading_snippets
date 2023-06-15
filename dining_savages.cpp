#include <thread>
#include <mutex>
#include <semaphore>
#include <iostream>
#include <vector>
#include <semaphore.h>
int serving = 10;
constexpr int MAX_SERVING = 10;
std::mutex streamMutex;
std::counting_semaphore<10> servingAvailable{0};
std::counting_semaphore<10> noServing{0};
std::counting_semaphore<10> servingMutex{1};

void printServing()
{
    {
        std::lock_guard guard(streamMutex);
        std::cout << serving << "\n";
    }
}

void getServingFromPot()
{
    serving--;
    printServing();
}

void putServingInPot()
{
    serving = MAX_SERVING;
    printServing();
}

void savage()
{
    while (true)
    {
        servingMutex.acquire();
        if (serving == 0)
        {
            noServing.release();
            servingAvailable.acquire();
        }

        getServingFromPot();
        servingMutex.release();
    }
}

void cook()
{
    while (true)
    {
        noServing.acquire();
        putServingInPot();
        servingAvailable.release();
    }
}

int main()
{
    std::vector<std::thread> threads;

    for (int i = 0; i < 10; i++)
    {
        threads.emplace_back(savage);
    }
    threads.emplace_back(cook);

    for (auto &thread : threads)
    {
        thread.join();
    }
}