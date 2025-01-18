#include "server.h"

int main() {
    server s = server("127.0.0.1", "8080");
    s.start();
}