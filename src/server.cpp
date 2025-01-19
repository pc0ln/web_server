#include "server.h"
#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <errno.h>
#include <thread>
#include <functional>

server::server(char* ip_addr, char* port) {
    struct addrinfo hints, *result, *rp;
    int status;
    int binded_socket;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo("127.0.0.1", "8080", &hints, &result)) != 0) {
        std::cerr << "Get address info error: " << gai_strerror(status) << std::endl;
        exit(0);
    }

    for (rp = result; rp != nullptr; rp = rp->ai_next) {
        // Just creating a socket
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1) {
            continue;
        }

        // Bind socket
        if ((binded_socket = bind(server_socket, rp->ai_addr, rp->ai_addrlen)) == 0) {
            break;
        }
    }
    freeaddrinfo(result);

    if (server_socket == -1) {
        std::cout << "Socket creation failed.\n";
        close(server_socket);
        exit(0);
   }

   if (binded_socket == -1) {
    std::cout << "Bind failed.\n";
    close(server_socket);
    exit(0);
   }
    
    // Listen
    if (listen(server_socket, 10) == -1) {
        std::cout << "Listening failed!\n";
        close(server_socket);
        exit(0);
    }

    std::cout << "Listening out on Port" << port << "\n";
}

int server::start() {
    while(true) {
        // Accept
        int client_fd = accept(server_socket, 0, 0);
        if (client_fd == -1) {
            std::cout << "Accept failed.\n";
            close(client_fd);
            break;
        }
        // Send Recieve
        std::cout << "Client connected." << std::endl;

        std::thread client(std::bind(&server::handle_client, this, client_fd));
        client.detach(); 
    }
    return 0;
}

server::~server() {
    // Clean up/Close
    close(server_socket);
    std::cout << "All cleaned up\n";
}

void server::handle_client(int fd) {
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

    char response[1024];
    handle_request(buffer, response);
    std::cout << "response: " << response << '\n';

    int bytes_sent = send(fd, response, strlen(response), 0);
    close(fd);
}

void server::handle_request(char* req, char* res) {
    if (strncmp(req, "GET", 3) != 0) {
        strncpy(res, "HTTP/1.1 400 Bad Request\r\n\r\n", 30);
    } else {
        char* body = stpncpy(res, "HTTP/1.1 200 OK\r\n\r\n", 20);
        char* file = req + 4;
        *strchr(file, ' ') = 0;
        response_builder(file, body, 20); 
    }
}

void server::response_builder(const char* file, char* res_body, int offset) {
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