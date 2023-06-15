#include <thread>
#include <mutex>
#include <semaphore>
#include <iostream>
#include <vector>
#include <cstdlib>

std::counting_semaphore tobacco{0};
std::counting_semaphore paper{0};
std::counting_semaphore match{0};
// std::counting_semaphore agentSem{1};
std::counting_semaphore resourceSem{1};

std::binary_semaphore smokerASem{0};
std::binary_semaphore smokerBSem{0};
std::binary_semaphore smokerCSem{0};
std::mutex streamMtx;

int tobaccoCount = 0;
int paperCount = 0;
int matchCount = 0;

void print(const std::string &str)
{
    std::lock_guard guard{streamMtx};
    std::cout << str << "\n";
    std::cout << "tobaccoCount: " << tobaccoCount << ", paperCount: " << paperCount << ", matchCount: " << matchCount << "\n";
}

void agent()
{
    while (true)
    {
        // agentSem.acquire();
        int agentId = rand() % 3;
        switch (agentId)
        {
        case 0:
            print("Agent: tobacco and paper");
            tobacco.release();
            paper.release();
            break;
        case 1:
            print("Agent: paper and match");
            paper.release();
            match.release();
            break;
        case 2:
            print("Agent: tobacco and match");
            tobacco.release();
            match.release();
            break;
        default:
            print("Invalid agents");
            throw std::invalid_argument("Invalid agents: " + std::to_string(agentId));
        }
    }
}

void pusherA()
{
    while (true)
    {
        tobacco.acquire();
        resourceSem.acquire();
        if (paperCount > 0)
        {
            paperCount--;
            smokerASem.release();
        }

        else if (matchCount > 0)
        {
            matchCount--;
            smokerCSem.release();
        }
        else
        {
            tobaccoCount++;
        }
        resourceSem.release();
    }
}

void pusherB()
{
    while (true)
    {
        paper.acquire();

        resourceSem.acquire();
        if (tobaccoCount > 0)
        {
            tobaccoCount--;
            smokerASem.release();
        }

        else if (matchCount > 0)
        {
            matchCount--;
            smokerBSem.release();
        }
        else
        {
            paperCount++;
        }
        resourceSem.release();
    }
}

void pusherC()
{
    while (true)
    {
        match.acquire();
        resourceSem.acquire();
        if (tobaccoCount > 0)
        {
            tobaccoCount--;
            smokerCSem.release();
        }

        else if (paperCount > 0)
        {
            paperCount--;
            smokerBSem.release();
        }
        else
        {
            matchCount++;
        }
        resourceSem.release();
    }
}

void smokerA()
{
    while (true)
    {
        smokerASem.acquire();
        print("SmokerA: tobacco and paper");
        // agentSem.release();
    }
}

void smokerB()
{
    while (true)
    {
        smokerBSem.acquire();
        print("SmokerB: paper and match");
        // agentSem.release();
    }
}

void smokerC()
{
    while (true)
    {
        smokerCSem.acquire();
        print("SmokerC: tobacco and match");
        // agentSem.release();
    }
}

int main()
{
    std::thread smokerAThread(smokerA);
    std::thread smokerBThread(smokerB);
    std::thread smokerCThread(smokerC);
    std::thread agentThread(agent);
    std::thread pusherAThread(pusherA);
    std::thread pusherBThread(pusherB);
    std::thread pusherCThread(pusherC);
    smokerAThread.join();
    smokerBThread.join();
    smokerCThread.join();
    agentThread.join();
    pusherAThread.join();
    pusherBThread.join();
    pusherCThread.join();
}