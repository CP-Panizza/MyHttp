#include <iostream>
#include <fstream>
#include <sstream>
#include "HttpServer.h"
#include "util.h"

int main(int argc, const char **argv) {
    HttpServer *server = new HttpServer(8080, 100, std::string(argv[0]));
    server->set_static_path("/static");

    server->bind_handle("GET", "/", [](Request req, Response *resp) {
        std::cout << "get a request" << std::endl;
        resp->set_header("Content-Type", "text/html");
        resp->write(200, "<html>Hello</html>");
    });

    server->bind_handle("POST", "/", [](Request req, Response *resp) {
        std::cout << "post run:"<< req.body << std::endl;
        resp->set_header("Content-Type", "text/html");
        resp->write(200, "Hello");
    });

    server->bind_handle("PUT", "/", [](Request req, Response *resp) {
        std::cout << "PUT run:"<< req.body << std::endl;
        resp->set_header("Content-Type", "text/html");
        resp->write(200, "Hello PUT!");
    });

    server->run();
    delete(server);
    return 0;
}