#include <thread>
#include <iostream>
#include <vector>
#include <semaphore>

constexpr int THREAD_COUNT = 5;
constexpr int LOOP_COUNT = 2;
class Printer
{
private:
    std::counting_semaphore<THREAD_COUNT> turnstile{0};
    std::counting_semaphore<THREAD_COUNT> turnstile2{1};
    std::mutex streamMtx;
    std::mutex counterMtx;
    int counter = 0;
    void printPreCritical()
    {
        std::lock_guard guard{streamMtx};
        std::cout << "Precritical " << std::this_thread::get_id() << "\n";
    }

    void printPostCritical()
    {
        std::lock_guard guard{streamMtx};
        std::cout << "Postcritical " << std::this_thread::get_id() << "\n";
    }

    void print(int num)
    {
        printPreCritical();
        {
            std::lock_guard{counterMtx};
            counter += 1;
            if (counter == THREAD_COUNT)
            {
                turnstile2.acquire();
                turnstile.release();
            }
        }
        turnstile.acquire();
        turnstile.release();

        printPostCritical();

        {
            std::lock_guard{counterMtx};
            counter -= 1;
            if (counter == 0)
            {
                turnstile.acquire();
                turnstile2.release();
            }
        }

        turnstile2.acquire();
        turnstile2.release();
    }

public:
    void operator()(int num)
    {
        for (int i = 0; i < LOOP_COUNT; i++)
        {
            print(num);
        }
    }

    void printSemaphoreState()
    {
        bool locked = !turnstile.try_acquire();
        std::cout << "Counter: " << counter << "\n"
                  << "Semaphore locked: " << locked << "\n";
    }
};

int main()
{
    std::vector<std::thread> threads;
    Printer printer;
    for (int i = 0; i < THREAD_COUNT; i++)
    {

        threads.emplace_back(std::thread(std::ref(printer), i));
    }

    for (auto &thread : threads)
    {
        thread.join();
    }
    printer.printSemaphoreState();
}