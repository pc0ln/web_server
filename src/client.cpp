#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
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

    // Get Server info
    if ((status = getaddrinfo("127.0.0.1", "8080", &hints, &result)) != 0) {
        std::cerr << "Get address info error: " << gai_strerror(status) << std::endl;
        return 1;
    }
    std::cout << "Get addrinfo worked!\n";

    // Socket creation
    int client_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (client_fd == -1) {
      std::cerr << "Socket Creation failed!\n";
      close(client_fd);
      return -1;
    }
    // Connecting to server
    if (connect(client_fd, result->ai_addr, result->ai_addrlen) == -1) {
      std::cerr << "Connection failed!\n";
      close(client_fd);
      return -1;
    }

    freeaddrinfo(result);
    std::cout << "Connected to server.\n";
    // Send Recieve
    char buffer[1024];
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate the message
        std::cout << "Server says: " << buffer << std::endl;
    } else {
        perror("Receive failed");
    }




    // Clean up/Close
    close(server_socket);
    std::cout << "All cleaned up\n";

    
    return 0;
}