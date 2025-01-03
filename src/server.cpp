#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
     // Find/Set up host address info
    struct addrinfo hints, *result, *rp;
    int status;
    int server_socket;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo("127.0.0.1", "8080", &hints, &result)) != 0) {
        std::cerr << "Get address info error: " << gai_strerror(status) << std::endl;
        return 1;
    }
    std::cout << "Get addrinfo worked!\n";

    for (rp = result; rp != nullptr; rp = rp->ai_next) {
        // Just creating a socket
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1) {
            continue;
        }

        // Bind socket
        if (bind(server_socket, rp->ai_addr, rp->ai_addrlen) == 0) {
            std::cout << "Bind Successful!\n";
            break;
        }

    }
    freeaddrinfo(result);
    if (server_socket == -1) {
        std::cout << "Socket creation failed.\n";
        close(server_socket);
        return -1;
   }
    
    // Listen
    if (listen(server_socket, 10) == -1) {
        std::cout << "Listening failed!\n";
        close(server_socket);
        return -1;
    }
    std::cout << "Listening out on Port 8080.\n";
    // Accept
    int client_fd = accept(server_socket, 0, 0);
    if (client_fd == -1) {
        std::cout << "Accept failed.\n";
        close(client_fd);
        close(server_socket);
        return -1;
    }
    // Send Recieve
    std::cout << "Client connected." << std::endl;

    char buffer[1024];
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate the message
        std::cout << buffer << std::endl;
    } else {
        perror("Receive failed");
    }




    // Clean up/Close
    close(server_socket);
    std::cout << "All cleaned up\n";

    
    return 0;
}