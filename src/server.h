#ifndef SERVER_HEADER
#define SERVER_HEADER
#include "threadpool.h"

class server
{
private:
    int server_socket;
    void handle_client(int fd);
    void handle_request(char* req, char* res);
    void response_builder(const char* file, char* res_body, int offset);

public:
    server(char* ip_addr, char* port);
    int start(threadpool *tpool);
    ~server();
};

#endif