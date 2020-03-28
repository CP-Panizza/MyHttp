//
// Created by cmj on 20-3-26.
//

#include  <stdio.h>
#include  <iostream>
#include  <unistd.h>
#include  <fcntl.h>
#include  <errno.h>
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <sys/epoll.h>
#include  <netinet/in.h>
#include  <arpa/inet.h>
#include <cstring>
#include <thread>
#include "HttpServer.h"
#include "Request.h"
#include "Response.h"

#define MAXLINE 4096

HttpServer::HttpServer(int port, int max_count):count(max_count) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(&server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(-1);
    }
    if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(-1);
    }
    //监听，设置最大连接数10
    if (listen(socket_fd, 10) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(-1);
    }
    //初始化事件列表
    epoll_events = new struct epoll_event[max_count];

    epoll_fd = epoll_create(max_count);
    struct epoll_event ev;
    ev.data.fd = socket_fd;
    ev.events  = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);
}


void HttpServer::run(int time_out)
{
    std::cout << "MyHttp started" << std::endl;
    while(1)
    {
        int ret = epoll_wait(epoll_fd, epoll_events, count, time_out);
        if(ret == 0)
        {
            continue;
        }
        else if(ret == -1)
        {
            printf("socket error: %s(errno: %d)\n", strerror(errno), errno);
        }
        else
        {
            for(int i = 0; i < ret; i++)
            {
                if(epoll_events[i].data.fd == socket_fd)
                {
                    sockaddr_in client_addr;
                    memset(&client_addr, 0, sizeof(client_addr));
                    socklen_t clilen = sizeof(struct sockaddr);
                    int conn = accept(socket_fd, (struct sockaddr*)&client_addr, &clilen);
//                    std::thread t(&HttpServer::Thread_handle, this, conn, std::string(inet_ntoa(client_addr.sin_addr)));
//                    t.detach();
                    this->Thread_handle(conn, std::string(inet_ntoa(client_addr.sin_addr)));
                }
            }
        }
    }
}


void HttpServer::Thread_handle(int conn, std::string) {
    Request request;
    char buf[MAXLINE];
    int n = recv(conn, buf, MAXLINE, 0);
    if(n == 0){
        close(n);
        return;
    } else if(n == -1){
        close(n);
        return;
    }
    buf[n] = '\0';
    try {
        request.Paser(std::string(buf));
    }catch (std::string err){
        std::cout << err << std::endl;
        close(conn);
        return;
    }
    Response response(conn);
    bool ok = false;
    std::function<void(Request, Response *)> temp_handle;
    if(methods.count(request.method)){
        auto l = methods[request.method];
        if(l->size()!=0){
            for(auto h : *l){
                if(h->url == request.path){
                    temp_handle = h->method;
                    ok = true;
                    break;
                }
            }
        }
    }
    if(ok){
        temp_handle(request, &response);
    }else {
        response.set_header("Content-Type","text/html");
        response.write(404,"Not found!");
    }
    close(conn);
}


void HttpServer::bind_handle(std::string method, std::string url, std::function<void(Request, Response *)> func) {
    auto h = new struct handle(url, func);
    if(methods.count(method)){
        methods[method]->push_back(h);
    } else {
        auto temp = new std::list<handle*>;
        temp->push_back(h);
        methods[method] = temp;
    }
}


HttpServer::~HttpServer() {

}

