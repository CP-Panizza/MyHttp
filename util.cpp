//
// Created by cmj on 20-3-26.
//

#include "util.h"


std::vector<std::string> split(std::string str,std::string pattern)
{
    std::string::size_type pos;
    std::vector<std::string> result;
    str+=pattern;//扩展字符串以方便操作
    int size=str.size();

    for(int i=0; i<size; i++)
    {
        pos=str.find(pattern,i);
        if(pos<size)
        {
            std::string s=str.substr(i,pos-i);
            result.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    return result;
}

bool contain(std::string str, std::string target){
    if(str == target)
        return true;
    if(str == "")
        return false;
    if(target == "")
        return true;
    std::string::size_type pos = str.find(target);
    if(pos == std::string::npos){
        return false;
    } else {
        return true;
    }
}