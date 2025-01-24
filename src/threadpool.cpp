#include "threadpool.h"
#include <errno.h>
#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

// Initalizing threadpool with number of worker threads
threadpool::threadpool(int threads) {
    for (int i = 0; i < threads; ++i) {
        //Create threads in worker vector
        workers.emplace_back([this] {
            while (true) {
                int client_fd;
                {
                    // Locks queue until wait to see if stop is true or a client is added
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    cv.wait(lock, [this] {return !client_list.empty() || stop;});
                    // If stopped
                    if (stop && client_list.empty()) {
                        return;
                    }
                    // Takes the client from queue
                    client_fd = client_list.front();
                    client_list.pop();
                }
                // Runs the client fd into handle function
                handle_client(client_fd);
            }
        });
    }
}

// Queuing clients to run handle client on
void threadpool::enqueue(int client_fd) {
    {
        // Locks queue to add client to list
        std::unique_lock<std::mutex> lock(queue_mutex);
        client_list.push(client_fd);
    }
    // Notifies a thread
    cv.notify_one();
}

threadpool::~threadpool() {
    // Remove threads
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }

    // Notifies all threads to join main thread ensuring completed task
    cv.notify_all();
    for (std::thread &worker: workers) {
        worker.join();
    }
}

void threadpool::handle_client(int fd) {
    //Fills recieved buffer from client
    char request[1024];
    ssize_t bytes_received = recv(fd, request, sizeof(request) - 1, 0);
    if (bytes_received > 0) {
        request[bytes_received] = '\0'; // Null-terminate the message
    } else {
        perror("Receive failed");
        close(fd);
        return;
    }
    // Send to handle request with extra buffer for response
    char response[1024];
    handle_request(request, response);
    
    // Sends response
    int bytes_sent = send(fd, response, strlen(response), 0);
    close(fd);
}

void threadpool::handle_request(char* req, char* res) {
    // If not get sends bad request
    if (strncmp(req, "GET", 3) != 0) {
        strncpy(res, "HTTP/1.1 400 Bad Request\r\n\r\n", 30);
    } else {
        //Send ok response before filling body
        char* body = stpncpy(res, "HTTP/1.1 200 OK\r\n\r\n", 20);
        char* file = req + 4;
        *strchr(file, ' ') = 0;
        response_builder(file, body, 20); 
    }
}

void threadpool::response_builder(const char* file, char* res_body, int offset) {
    //If file isnt root the echos to client
    if (strcmp(file, "/") == 0) {
        int opened_fd = open("../src/index.html", O_RDONLY);
        if (opened_fd == -1) {
            std::cout << strerror(errno) << "\n";
        }
        int bytes_read = read(opened_fd, res_body, 1024 - offset);
    } else {
        strncpy(res_body, file, 1024 - offset);
    }
}