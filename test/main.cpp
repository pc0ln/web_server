// TODO: web server
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    // Checks count of arguments
    if (argc != 2) {
        printf("Usage: test <hostname>");
        return 1;
    }

    // Address info structs for result, temp pointer, hints
    struct addrinfo hints, *res, *p;
    // Status to make sure the get address info doesn't fail
    int status;
    // Buffer for ip address as a string
    char ip_string[INET6_ADDRSTRLEN];

    // Sets hints to all 0's
    memset(&hints, 0, sizeof(hints));
    // Unspecified so open to v4, and v6
    hints.ai_family = AF_UNSPEC;
    // TDP socket stream so makes sure all data is conserved
    hints.ai_socktype = SOCK_STREAM;

    // Checks to make sure the function doesnt fail
    if ((status = getaddrinfo(argv[1], nullptr, &hints, &res)) != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return 2;
    }
    // Header
    std::cout << "IP Address for " << argv[1] << ":" << std::endl;
    // Loops through linked list to find first ip address
    for (p = res; p != nullptr; p = p->ai_next) {
        // Pointer to address of socket
        void* address;
        // String for ip version
        std::string ip_version;

        // If IPv4
        if (p->ai_family == AF_INET) {
            //Sets version and fills Ipv4 sockaddr struct and addreess
            ip_version = "IPv4";
            struct sockaddr_in* ipv4 = (struct sockaddr_in *)p->ai_addr;
            address = &(ipv4->sin_addr);
        // If IPv6
        } else {
            //Sets version and fills Ipv6 sockaddr struct and addreess
            ip_version = "IPv6";
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            address = &(ipv6->sin6_addr);
        }

        // Network to printable lol, Function to take binary to ip string 
        inet_ntop(p->ai_family, address, ip_string, sizeof(ip_string));
        std::cout << ip_version << ":" << ip_string << std::endl;
    }
    // Frees linked list
    freeaddrinfo(res);

    return 0;
}