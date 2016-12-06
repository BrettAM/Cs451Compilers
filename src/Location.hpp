#ifndef LOCATION_H
#define LOCATION_H

#include <assert.h>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <stdlib.h>
#include <string>

#include "Registers.hpp"

class MemoryRef{
public:
    enum MemoryRegion { PROGRAM, DATA } region;
    int offset;
    int registr;
private:
    MemoryRef(MemoryRegion mr, int of, int rg):
        region(mr), offset(of), registr(rg) { }
public:
    static MemoryRef Program(int offset, int registerNumber){
        return MemoryRef(PROGRAM, offset, registerNumber);
    }
    static MemoryRef Program(int offset){
        return MemoryRef(PROGRAM, offset, ZEROREG);
    }
    static MemoryRef Data(int offset, int registerNumber){
        return MemoryRef(DATA, offset, registerNumber);
    }
    static MemoryRef Data(int offset){
        return MemoryRef(DATA, offset, ZEROREG);
    }
    bool operator==(const MemoryRef& r) const {
        return region == r.region && offset == r.offset && registr == r.registr;
    }
};

struct AlreadyBoundException : public std::exception {
    const char * what () const throw () {
        return "C++ Exception";
    }
};

struct NotYetBoundException : public std::exception {
   const char * what () const throw () {
      return "Attempt to lookup an unbound location";
   }
};

class Location{
private:
    enum BindState { FREE, VALUE, REFERENCE } bound;
    MemoryRef value;
    Location* reference;
public:
    Location():
        bound(FREE), value(MemoryRef::Data(-1,-1)), reference(NULL){}
    Location(MemoryRef value):
        bound(VALUE), value(value), reference(NULL) {}
    Location(Location* reference):
        bound(REFERENCE), value(MemoryRef::Data(-1,-1)), reference(reference){}
    void bind(MemoryRef loc){
        if(bound != FREE) throw AlreadyBoundException();
        bound = VALUE;
        value = loc;
    }
    void bind(Location* ref){
        if(bound != FREE) throw AlreadyBoundException();
        bound = REFERENCE;
        reference = ref;
    }
    MemoryRef lookup() const {
        switch(bound){
            case VALUE:
                return value;
            case REFERENCE:
                return reference->lookup();
            default:
                throw NotYetBoundException();
        }
    }
};


#endif
