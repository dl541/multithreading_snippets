#include <semaphore>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
constexpr int THREAD_COUNT = 10;
constexpr int CAPACITY = 5;
std::mutex streamMutex;

class CircularQueue
{
private:
    int increment(int ind)
    {
        int size = arr.size();
        return (ind + 1) % size;
    }

public:
    std::vector<int> arr;
    int start = 0;
    int end = 0;
    CircularQueue(int capacity) : arr(std::vector<int>(capacity + 1))
    {
    }

    bool push(int num)
    {
        if (full())
        {
            return false;
        }
        arr[end] = num;
        end = increment(end);
        return true;
    }

    int pop()
    {
        if (empty())
        {
            return -1;
        }
        int res = arr[start];
        start = increment(start);
        return res;
    }

    bool empty()
    {
        return start == end;
    }

    bool full()
    {
        return increment(end) == start;
    }
};

class BoundedBlockingQueue
{
public:
    CircularQueue q;
    std::counting_semaphore<10> space;
    std::binary_semaphore readyToPop{0};
    std::mutex queueMtx;
    BoundedBlockingQueue(int capacity) : q(capacity), space(capacity)
    {
    }

    int pop()
    {
        readyToPop.acquire();
        int ans = INT_MAX;
        {
            // std::lock_guard guard(queueMtx);
            ans = q.pop();
        }
        space.release();
        return ans;
    }

    void push(int num)
    {
        space.acquire();
        {
            // std::lock_guard guard(queueMtx);
            q.push(num);
        }
        readyToPop.release();
    }
};

void produce(BoundedBlockingQueue &queue, int num)
{
    queue.push(num);
}

void consume(BoundedBlockingQueue &queue)
{
    {
        std::lock_guard guard(streamMutex);
        int res = queue.pop();
        std::cout << "Popped result: " << res << "\n";
    }
}

int main()
{
    BoundedBlockingQueue queue{CAPACITY};
    std::vector<std::thread> threads;
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        threads.emplace_back(produce, std::ref(queue), i);
    }

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        threads.emplace_back(consume, std::ref(queue));
    }

    for (auto &thread : threads)
    {
        thread.join();
    }
}