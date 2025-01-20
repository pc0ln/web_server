#include "server.h"

int main() {
    //Creates instance of server with local ip and port 8080 then starts
    server s = server("127.0.0.1", "8080");
    s.start();
}