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
        //Create threads but lock them
        workers.emplace_back([this] {
            while (true) {
                int client_fd;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    cv.wait(lock, [this] {return !task_list.empty() || stop;});
                    if (stop && task_list.empty()) {
                        std::cout << "exiting\n";
                        return;
                    }
                    std::cout << "looking at queue\n";
                    client_fd = task_list.front();
                    task_list.pop();
                }
                std::cout << "sending to handle function\n";
                handle_client(client_fd);
            }
        });
    }
}

// Queuing clients to run handle client on
void threadpool::enqueue(int client_fd) {
    std::cout << "queued\n";
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        task_list.push(client_fd);
    }
    std::cout << "tasks: " << task_list.size() << "\n";
    cv.notify_one();
}

threadpool::~threadpool() {
    std::cout << "deconstructed\n";
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

void threadpool::handle_client(int fd) {
    //Fills recieved buffer from client
    std::cout << "handling client\n";
    char buffer[1024];
    ssize_t bytes_received = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate the message
        std::cout << buffer << std::endl;
    } else {
        perror("Receive failed");
        close(fd);
        return;
    }
    // Send to handle request with extra buffer for response
    char response[1024];
    handle_request(buffer, response);
    std::cout << "response: " << response << '\n';
    
    // Sends response
    int bytes_sent = send(fd, response, strlen(response), 0);
    std::cout << "sent and closing client\n";
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