#include "threadpool.h"

// Initalizing threadpool with number of worker threads
threadpool::threadpool(int threads) {
    for (int i = 0; i < threads; ++i) {
        //Create threads but lock them
        workers.emplace_back([this] {
            while (true) {
                int client_fd;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    cv.wait(lock, [this] {return !task_list.empty() || stop;});
                    if (stop && task_list.empty()) {
                        return;
                    }
                    client_fd = task_list.front();
                    task_list.pop();
                }
                handleClient(client_fd);
            }
        });
    }
}

// Queuing clients to run handle client on
void threadpool::enqueue(int client_fd) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        task_list.push(client_fd);
    }
    cv.notify_one();
}

threadpool::~threadpool() {
    // Remove threads
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }

    cv.notify_all();
    for (std::thread &worker: workers) {
        worker.join();
    }
}