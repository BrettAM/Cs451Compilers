#ifndef LOCATION_H
#define LOCATION_H

#include <iostream>
#include <sstream>
#include <stdint.h>
#include <stdlib.h>
#include <string>

class Location{
public:
    enum RefClass {NONE, GLOBAL, STATIC, PARAMETER, LOCAL};
    Location():
        hidden(true) {}
    Location(RefClass ref, int size, int location):
        hidden(false), ref(ref), size(size), location(location) {}
    int getSize() { return size; }
    std::string textBox() const {
        if(hidden) return "";

        std::ostringstream oss;
        oss << "[ref: ";
        switch(ref){
            case NONE:      oss << "None"; break;
            case GLOBAL:    oss << "Global"; break;
            case STATIC:    oss << "Static"; break;
            case PARAMETER: oss << "Param"; break;
            case LOCAL:     oss << "Local"; break;
        }
        oss << ", size: " << size;
        oss << ", loc: " << location;
        oss << "] ";
        return oss.str();
    }
private:
    bool hidden;
    RefClass ref;
    int size;
    int location;
};

#endif
