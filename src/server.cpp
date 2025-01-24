#include "server.h"
#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

server::server(char* ip_addr, char* port) {
    struct addrinfo hints, *result, *rp;
    int status;
    int binded_socket;

    // Sets address info struct to 0
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    //Gets address info basically fills struct
    if ((status = getaddrinfo("127.0.0.1", "8080", &hints, &result)) != 0) {
        std::cerr << "Get address info error: " << gai_strerror(status) << std::endl;
        exit(0);
    }

    // Loops through linked list off addresses
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
    //Frees linked list
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

// Starts server to accept clients in loop
int server::start(threadpool *tpool) {
    while(true) {
        // Accept
        int client_fd = accept(server_socket, 0, 0);
        if (client_fd == -1) {
            std::cout << "Accept failed.\n";
            close(client_fd);
            break;
        }

        // Adds accepted client to thread pool list
        tpool->enqueue(client_fd);
    }
    return 0;
}

server::~server() {
    // Clean up/Close
    close(server_socket);
}
