#include <thread>
#include <iostream>
#include <vector>
#include <semaphore>

constexpr int THREAD_COUNT = 5;
std::mutex streamMtx;

std::counting_semaphore followerSem{0};
std::counting_semaphore leaderSem{0};

void print(std::string str)
{
    std::lock_guard guard(streamMtx);
    std::cout << str << "\n";
}

void printLeader(int num)
{
    followerSem.release();
    leaderSem.acquire();
    print("Leader " + std::to_string(num));
}

void printFollower(int num)
{
    leaderSem.release();
    followerSem.acquire();
    print("Follower " + std::to_string(num));
}

int main()
{
    std::vector<std::thread> threads;

    for (int i = 0; i < THREAD_COUNT; i++)
    {

        threads.emplace_back(std::thread(printLeader, i));
    }

    for (int i = 0; i < THREAD_COUNT; i++)
    {

        threads.emplace_back(std::thread(printFollower, i));
    }

    for (auto &thread : threads)
    {
        thread.join();
    }
}