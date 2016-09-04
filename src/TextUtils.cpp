#include "TextUtils.hpp"

char TextUtils::unescape(cstr s){
    if(s[0]=='\0' || s[1]=='\0' || s[0]!='\\') return s[0];
    switch(s[1]){
        case '0': return '\0';
        case 'n': return '\n';
        default:  return s[1];
    }
}

std::string TextUtils::strip(cstr s){
    std::string str(s);
    return str.substr(1,str.length()-2);
}

