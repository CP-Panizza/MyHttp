//
// Created by cmj on 20-3-26.
//

#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H
#include <map>


class Response {
private:
    int conn;
    int code;
    std::map<std::string,std::string> header;
    std::string get_descript(int code);

public:
    Response(int fd):conn(fd){}
    void set_header(std::string, std::string);
    void write(int,std::string);

};


#endif //HTTP_RESPONSE_H
