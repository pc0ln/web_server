#ifndef THREADPOOL
#define THREADPOOL

#include <queue>
#include <thread>
#include <condition_variable>
#include <mutex>

class threadpool
{
private:
    std::queue<int> task_list;
    std::vector<std::thread> workers;
    std::condition_variable cv;
    std::mutex queue_mutex;
    bool stop;
public:
    threadpool::threadpool(int threads);
    threadpool::~threadpool();
    void enqueue(int client_fd);
    void handle_client(int fd);
};

#endif