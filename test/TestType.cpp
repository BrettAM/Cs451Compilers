#include "UnitTest++/UnitTest++.h"
#include "Type.hpp"
#include <string>
#include <sstream>

TEST(makeArray){
    Type t = Type::BOOL.mkArray(0);
    CHECK_EQUAL(true, t.isArray());
    CHECK_EQUAL(false, Type::BOOL.isArray());
}

TEST(makeStatic){
    Type t = Type::INT.mkStatic();
    CHECK_EQUAL(true, t.isStatic());
    CHECK_EQUAL(false, Type::BOOL.isStatic());
}

TEST(PrintBasic){
    CHECK_EQUAL(" of type bool",
                Type::BOOL.toString()
                );
    CHECK_EQUAL(" of type int",
                Type::INT.toString()
                );
    CHECK_EQUAL(" of type void",
                Type::VOID.toString()
                );
    CHECK_EQUAL(" of type record",
                Type::RECORD().toString()
                );
}

TEST(PrintStatic){
    CHECK_EQUAL(" is static of type bool",
                Type::BOOL.mkStatic().toString()
                );
}

TEST(PrintArray){
    CHECK_EQUAL(" is array of type int",
                Type::INT.mkArray(0).toString()
                );
}

TEST(PrintStaticArray){
    CHECK_EQUAL(" is static array of type bool",
                Type::BOOL.mkArray(0).mkStatic().toString()
                );
}
