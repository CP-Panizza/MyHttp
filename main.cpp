#include <iostream>
#include <fstream>
#include <sstream>
#include "HttpServer.h"
#include "util.h"

class MyHandle {
public:
    int num;
    void Hello(Request req, Response *resp) {
        std::ifstream f("../LICENSE");
        std::ostringstream tmp;
        tmp << f.rdbuf();
        std::string str = tmp.str();
        resp->write(200, str);
    }
};


int main(int argc, const char **argv) {
    std::cout << argv[0] << std::endl;
    MyHandle *myHandle = new MyHandle;
    HttpServer *server = new HttpServer(8080, 100, std::string(argv[0]));
    server->set_static_path("/static");
//    server->bind_handle("GET", "/", [](Request req, Response *resp) {
//        std::cout << "get a request" << std::endl;
//        resp->set_header("Content-Type", "text/html");
//        resp->write(200, "Hello");
//    });
//
//    server->bind_handle("POST", "/", [](Request req, Response *resp) {
//        std::cout << req.body << std::endl;
//        resp->set_header("Content-Type", "text/html");
//        resp->write(200, "Hello");
//    });
//
//    server->bind_handle("GET", "/c",
//                        std::bind(&MyHandle::Hello, myHandle, std::placeholders::_1, std::placeholders::_2));
//
    server->run(2000);
    return 0;
}