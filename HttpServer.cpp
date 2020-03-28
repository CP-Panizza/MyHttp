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
    int ret;
    while(1)
    {
        ret = epoll_wait(epoll_fd, epoll_events, count, time_out);
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
                    //接受新的链接，并设置为非阻塞
                    do_accept(socket_fd, epoll_fd);
                } else {
                    Thread_handle(epoll_events[i].data.fd);
                }
            }
        }
    }
}

// 断开连接的函数
void HttpServer::disconnect(int cfd){
    int ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, cfd, NULL);
    if(ret == -1){
        perror("epoll_ctl del cfd error");
        return;
    }
    close(cfd);
}


void HttpServer::Thread_handle(int conn) {
    Request request;
    char buf[MAXLINE];
    int n = recv(conn, buf, MAXLINE, 0);
    if(n == 0){
        disconnect(conn);
        return;
    } else if(n == -1){
        disconnect(conn);
        return;
    }
    buf[n] = '\0';
    try {
        request.Paser(std::string(buf));
    }catch (std::string err){
        std::cout << err << std::endl;
        disconnect(conn);
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
    disconnect(conn);
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



//
//template<class T>
//void HttpServer::bind_handle(std::string method, std::string url, T* obj, std::function<void(T::*)(Request, Response*)> *func){
//    auto f = std::bind(func, obj, std::placeholders::_1,std::placeholders::_2);
//    auto h = new struct handle(url, func);
//    if(methods.count(method)){
//        methods[method]->push_back(h);
//    } else {
//        auto temp = new std::list<handle*>;
//        temp->push_back(h);
//        methods[method] = temp;
//    }
//}

HttpServer::~HttpServer() {

}

void HttpServer::do_accept(int socket_fd, int epoll_fd) {
    struct sockaddr_in cli;
    socklen_t len = sizeof(cli);
    int new_fd = accept(socket_fd, (struct sockaddr*)&cli, &len);
    if(new_fd == -1){
        perror("accept err");
        exit(1);
    }

    //set nonblock socket
    int flag = fcntl(new_fd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(new_fd, F_SETFL, flag);

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = new_fd;
    int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_fd, &ev);
    if(ret == -1){
        perror("epoll_ctl err");
        exit(1);
    }
}

