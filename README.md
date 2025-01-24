# Multithreaded HTTP Server

Minimal multithreaded HTTP web server implemented with a threadpool

## Description

Minimal HTTP server that can run multiple clients concurrently. If root file then displays an index html, otherwise echos file to client.

## Usage

```cpp
#include "server.h"
#include "threadpool.h"

// Insert number of threads for x
threadpool tp = threadpool(x)
// Insert IP4 address and port as strings
server s = server("ip address", "port");
// Run method start server with address of threadpool instance
s.start(&tp);
```

## License

[MIT](https://choosealicense.com/licenses/mit/)