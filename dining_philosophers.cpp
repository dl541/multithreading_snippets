#include <thread>
#include <mutex>
#include <semaphore>
#include <iostream>
#include <vector>
#include <chrono>
#include <unordered_set>

constexpr static int THREAD_COUNT = 1000;
static std::pair<int, int> vals{0, 0};

static std::atomic_int32_t readCount = 0;
std::mutex streamMtx;

std::vector<int> ans;

std::atomic_int32_t readerCount = 0;
std::binary_semaphore turnstile{1}; // Not starving writers
std::binary_semaphore valsMutex{1};

class LightSwitch
{
private:
    int counter = 0;
    std::binary_semaphore mutex{1};

public:
    void lock(std::binary_semaphore &semaphore)
    {
        mutex.acquire();
        counter += 1;
        if (counter == 1)
        {
            semaphore.acquire();
        }
        mutex.release();
    }

    void unlock(std::binary_semaphore &semaphore)
    {
        mutex.acquire();
        counter -= 1;
        if (counter == 0)
        {
            semaphore.release();
        }
        mutex.release();
    }
};

LightSwitch readerSwitch;

namespace ReaderWriter
{
    void write()
    {
        turnstile.acquire();
        valsMutex.acquire();
        vals.first += 1;
        vals.second += 1;
        valsMutex.release();
        turnstile.release();
    }

    void read()
    {
        turnstile.acquire();
        turnstile.release();
        readerSwitch.lock(valsMutex);
        {
            std::lock_guard guard(streamMtx);
            std::cout << "Read count: " << readCount << " Vals: " << vals.first << " " << vals.second << "\n";
            readCount += 1;
            ans.push_back(vals.first + vals.second);
        }
        readerSwitch.unlock(valsMutex);
    }
}

int main()
{
    std::vector<std::thread> threads;

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        threads.emplace_back(ReaderWriter::read);
        if (i % 5 == 0)
        {
            threads.emplace_back(ReaderWriter::write);
        }
    }

    for (auto &thread : threads)
    {
        thread.join();
    }
}