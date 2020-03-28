cmake_minimum_required(VERSION 3.11)
project(http)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
add_executable(MyHttp main.cpp Request.cpp Request.h util.cpp util.h HttpServer.cpp HttpServer.h Response.cpp Response.h)