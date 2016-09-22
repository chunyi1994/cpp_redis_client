#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <sstream>
static std::string int2string(const int i){
    std::stringstream is;
    is<<i;
    return is.str();
}

static int string2int(const std::string& s){
    if(s.length() == 0){
        return 0;
    }
    std::istringstream is(s);
    int ret = 0;
    is>>ret;
    return ret;
}
#endif // UTILS_H

