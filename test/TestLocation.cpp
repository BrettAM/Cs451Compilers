#include "UnitTest++/UnitTest++.h"
#include "Location.hpp"
#include <string>
#include <sstream>
#include <exception>
using namespace std;

TEST(lookupPointer){
    MemoryRef mr = MemoryRef::Data(1,2);
    Location rf(mr);
    Location loc(&rf);
    CHECK(loc.lookup() == mr);
}
TEST(lookupValue){
    MemoryRef mr = MemoryRef::Data(1,2);
    Location loc(mr);
    CHECK(loc.lookup() == mr);
}
TEST(lookupBad){
    Location loc;
    bool exceptionThrown = false;
    try{
        loc.lookup();
    } catch (NotYetBoundException& e) {
        exceptionThrown = true;
    }
    CHECK(exceptionThrown);
}
TEST(BindPointer){
    MemoryRef mr = MemoryRef::Data(1,2);
    Location rf(mr);
    Location loc;
    loc.bind(&rf);
    CHECK(loc.lookup() == mr);
}
TEST(BindValue){
    MemoryRef mr = MemoryRef::Data(1,2);
    Location loc;
    loc.bind(mr);
    CHECK(loc.lookup() == mr);
}
TEST(BindBad){
    MemoryRef mr = MemoryRef::Data(1,2);
    Location loc(mr);
    bool exceptionThrown = false;
    try{
        loc.bind(mr);
    } catch (AlreadyBoundException& e) {
        exceptionThrown = true;
    }
    CHECK(exceptionThrown);
}
TEST(ConstructorPointer){
    Location rf;
    Location loc(&rf);
}
TEST(ConstructorValue){
    MemoryRef mr = MemoryRef::Data(1,2);
    Location loc(mr);
}
TEST(ConstructorNone){
    Location loc;
}
