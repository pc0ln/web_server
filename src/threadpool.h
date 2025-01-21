#ifndef THREADPOOL
#define THREADPOOL
#include <queue>

class threadpool
{
private:
    std::queue<int> task_list;   
public:
    threadpool::threadpool(int threads);
    threadpool::~threadpool();
    void queue(int client_fd);
};

#endif