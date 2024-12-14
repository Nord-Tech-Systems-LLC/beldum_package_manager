# Create new project

```
beldum create cpp_webserver_test && cd cpp_webserver_test
beldum install cpp_webserver
```

### Paste the following in `main.cpp`:

```cpp
#include "cpp_webserver_include/core.hpp"

int main()
{
    HttpServer server("127.0.0.1", "8080");

    server.get("/", [](Request &req, Response &res)
               { res.send("Hello, world!"); });

    server.get(
        "/json", [&](Request &req, Response &res)
        { res.status(200).json("{\"message\": \"Hello, JSON\"}"); });

    server.post("/submit", [](Request &req, Response &res)
                {
        // Handle POST data
        res.status(201).send("Submitted!"); });
    server.printRoutes();
    server.start();

    return 0;
}
```

# Build project:
```
beldum run build
beldum run execute
```