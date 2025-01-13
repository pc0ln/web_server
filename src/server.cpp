#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <errno.h>

void handle_request(char* req, char* res);
int http_parser(char *buffer);
void response_builder(char* res_body, int offset);

int main(int argc, char* argv[]) {
    // Find/Set up host address info
    struct addrinfo hints, *result, *rp;
    int status;
    int server_socket, binded_socket;

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
        if ((binded_socket = bind(server_socket, rp->ai_addr, rp->ai_addrlen)) == 0) {
            std::cout << "Bind Successful!\n";
            break;
        } else {
            std::cout << "Bind failed.\n";
        }

    }
    freeaddrinfo(result);

    if (server_socket == -1) {
        std::cout << "Socket creation failed.\n";
        close(server_socket);
        return -1;
   }

   if (binded_socket == -1) {
    std::cout << "Bind failed.\n";
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

    char response[1024];
    handle_request(buffer, response);
    std::cout << "response: " << response << '\n';

    // int valid = http_parser(buffer);
    
    // char *reply;
    // if (valid != 0) {
    //     reply = "HTTP/1.1 400 Bad Request\r\n\r\n";
    // } else {
    //     response_builder(client_fd);
    //     reply = "HTTP/1.1 200 OK\r\n\r\n";
    // }
    
    int bytes_sent = send(client_fd, response, strlen(response), 0);




    // Clean up/Close
    close(server_socket);
    std::cout << "All cleaned up\n";

    
    return 0;
}

void handle_request(char* req, char* res) {
    if (http_parser(req) != 0) {
        strncpy(res, "HTTP/1.1 400 Bad Request\r\n\r\n", 30);
    } else {
        char* body = stpncpy(res, "HTTP/1.1 200 OK\r\n", 18);
        response_builder(body, 18); 
    }
}

int http_parser(char *buffer) {
    int i = 0;
    while (!isspace(buffer[i])) {
        i++;
    }
    buffer[i] = '\0';
    std::cout << buffer << '\n';
    return strcmp(buffer, "GET");
}

void response_builder(char* res_body, int offset) {
    int opened_fd = open("../src/index.html", O_RDONLY);
    if (opened_fd == -1) {
        std::cout << strerror(errno) << "\n";
    }
    int bytes_read = read(opened_fd, res_body, 1024 - offset);
    //std::cout << "bytes: " << bytes_read << " body: " << res_body << "\n";
    //std::cout << strerror(errno) << "\n";
    res_body[bytes_read++] = '\r';
    res_body[bytes_read] = '\n';
}