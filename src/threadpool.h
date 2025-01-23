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
    bool stop = false;
public:
    threadpool(int threads);
    ~threadpool();
    void enqueue(int client_fd);
    void handle_client(int fd);
    void handle_request(char* req, char* res);
    void response_builder(const char* file, char* res_body, int offset);
};

#endif