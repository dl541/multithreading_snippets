#include <thread>
#include <iostream>
#include <vector>
#include <semaphore>

constexpr int THREAD_COUNT = 5;
std::mutex streamMtx;

std::counting_semaphore followerQueue{0};
std::counting_semaphore leaderQueue{0};
std::binary_semaphore mtx{1};
std::binary_semaphore rendezvous{0};
int leaders = 0;
int followers = 0;

void print(std::string str)
{
    std::lock_guard guard(streamMtx);
    std::cout << str << "\n";
}

void dance()
{
}

void printLeader(int num)
{
    mtx.acquire();
    if (followers > 0)
    {
        followers--;
        followerQueue.release();
    }
    else
    {
        leaders++;
        mtx.release();
        leaderQueue.acquire();
    }

    rendezvous.acquire();
    print("Leader " + std::to_string(num));
    mtx.release();
}

void printFollower(int num)
{
    mtx.acquire();
    if (leaders > 0)
    {
        leaders--;
        leaderQueue.release();
    }
    else
    {
        followers++;
        mtx.release();
        followerQueue.acquire();
    }
    print("Follower " + std::to_string(num));
    rendezvous.release();
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