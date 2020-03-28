//
// Created by cmj on 20-3-26.
//

#ifndef HTTP_UTIL_H
#define HTTP_UTIL_H


#include <string>
#include <vector>
#include <fstream>
#include <sstream>

std::vector<std::string> split(std::string str, std::string pattern);
bool contain(std::string str, std::string target);
inline bool fiel_exists (const std::string& name);

#endif //HTTP_UTIL_H
