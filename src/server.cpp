#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
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
            std::cout << "Bind Successful!";
            break;
        }

    }
    freeaddrinfo(result);
    if (server_socket == -1) {
        std::cout << "Socket creation failed.\n";
        return 1;
   }
   std::cout << "Socket created!\n";
    
    // Listen

    // Accept

    // Send Recieve

    // Clean up/Close
    close(server_socket);
    std::cout << "All cleaned up\n";

    
    return 0;
}