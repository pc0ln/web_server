#include "threadpool.h"

// Initalizing threadpool with number of worker threads
threadpool::threadpool(int threads) {
    for (int i = 0; i < threads; ++i) {
        //Create threads but lock them
    }
}

// Queuing clients to run handle client on
void threadpool::queue(int client_fd) {
    task_list.push(client_fd);
}

threadpool::~threadpool() {
    // Remove threads
}