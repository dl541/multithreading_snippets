#include <queue>
#include <string>
#include <vector>
#include <set>
#include <thread>
#include <semaphore.h>
#include <mutex>
class HtmlParser
{
public:
    std::vector<std::string> getUrls(std::string url) const;
};
class Solution
{
private:
    sem_t empty;
    std::mutex mtx;

public:
    std::queue<std::string> queue;
    std::set<std::string> set;

    Solution()
    {
        sem_init(&empty, 0, 1);
    }
    std::vector<std::string> crawl(std::string startUrl, HtmlParser htmlParser)
    {
        queue.push(startUrl);
        set.insert(startUrl);
        std::vector<std::string> crawlResult;

        return crawlResult;
    }

    void bfs(std::vector<std::string> &crawlResult, const HtmlParser &htmlParser)
    {
        sem_wait(&empty);
        {
            std::lock_guard guard(mtx);
            std::string url = queue.front();
            queue.pop();
            crawlResult.push_back(url);
            for (auto nextUrls : htmlParser.getUrls(url))
            {
                if (set.count(nextUrls) == 0)
                {
                    queue.push(nextUrls);
                    set.insert(nextUrls);
                    std::thread threadObj(bfs, std::ref(crawlResult), std::ref(htmlParser));
                    threadObj.detach();
                                }
            }
        }
        sem_post(&empty);
    }
};