#include <iostream>
#include "HttpServer.h"


int main() {
    HttpServer *server = new HttpServer(8080, 10);
    server->bind_handle("GET", "/", [](Request req, Response *resp){
        resp->set_header("Content-Type","text/html");
        resp->write(200, "Hello");
    });

    server->bind_handle("POST", "/", [](Request req, Response *resp){
        std::cout<< req.body << std::endl;
        resp->set_header("Content-Type","text/html");
        resp->write(200, "Hello");
    });

    server->run(2000);
    return 0;
}