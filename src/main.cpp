#include "server.h"
#include "threadpool.h"

int main() {
    // Creating threadpool with 10 threads
    threadpool tp = threadpool(10);
    // Creating server with local ip and port 8080
    server s = server("127.0.0.1", "8080");
    // Starting server with threadpool
    s.start(&tp);

    return 0;
}