//
// Created by cmj on 20-3-26.
//

#include <iostream>
#include <vector>
#include "Request.h"
#include "util.h"


void Request::Paser(std::string data) {
    bool has_body = false;
    bool has_url_param = false;
    auto all = split(data, "\r\n");
    auto temp = all[0];
    auto line = split(temp, " ");
    if (line.size() != 3) {
        throw std::string("data err!");
    }
    this->method = line[0];
    this->url = line[1];
    this->http_version = line[2];
    auto v_path = split(line[1], "?");
    if (v_path.size() == 2) {
        this->path = v_path[0];
        has_url_param = true;
    } else if (v_path.size() == 1) {
        this->path = url;
    }

    if (has_url_param) {
        auto p = split(v_path[1], "&");
        for (auto x : p) {
            auto param = split(x, "=");
            if (param.size() == 2) {
                params[param[0]] = param[1];
            }
        }
    }

    if (this->method == "GET" || this->method == "DELETE") {
        for (int i = 1; i < all.size(); ++i) {
            if (all[i] != "" && contain(all[i], ":")) {
                auto head = split(all[i], ":");
                if (head.size() == 2) {
                    this->header[head[0]] = head[1];
                }
            }
        }
    } else {
        auto body_index = data.find("\r\n\r\n") + 4;
        if (body_index == data.size()) { //无请求体
            for (int i = 1; i < all.size(); ++i) {
                if (all[i] != "" && contain(all[i], ":")) {
                    auto head = split(all[i], ":");
                    if (head.size() == 2) {
                        this->header[head[0]] = head[1];
                    }
                }
            }
        } else {
            std::cout << "q" << std::endl;
        }
    }
}
