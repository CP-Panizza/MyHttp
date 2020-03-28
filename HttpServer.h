//
// Created by cmj on 20-3-26.
//

#ifndef HTTP_HTTPSERVER_H
#define HTTP_HTTPSERVER_H

#include <functional>
#include <map>
#include "Request.h"
#include "Response.h"
#include <list>

struct handle{
    handle(std::string u, std::function<void(Request, Response *)> m):url(u), method(m){}
    std::string url;
    std::function<void(Request, Response *)> method;
};

class HttpServer {
public:
    HttpServer(int port, int max_count, std::string);
    ~HttpServer();
    void do_accept(int socket_fd, int epoll_fd);
    void disconnect(int cfd);
    void bind_handle(std::string method, std::string url, std::function<void(Request, Response*)> func);
    void Thread_handle(int conn);
    void run(int time_out);

private:
    std::string excute_pwd; //程序启动路径
    int socket_fd;
    struct epoll_event *epoll_events;
    int epoll_fd;
    int count;
    std::map<std::string, std::list<handle*>*> methods;
};


#endif //HTTP_HTTPSERVER_H
