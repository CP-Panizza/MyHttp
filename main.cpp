#include <iostream>
#include <fstream>
#include <sstream>
#include "HttpServer.h"
#include "util.h"

int a = 0;

int main(int argc, const char **argv) {
    std::cout << argv[0] << std::endl;
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

    server->run();
    return 0;
}