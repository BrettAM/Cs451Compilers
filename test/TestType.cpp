#include "UnitTest++/UnitTest++.h"
#include "Type.hpp"
#include <string>
#include <sstream>

TEST(makeArray){
    Type t = Type::BOOL.asArray(0);
    CHECK_EQUAL(true, t.isArray());
    CHECK_EQUAL(false, Type::BOOL.isArray());
}

TEST(makeStatic){
    Type t = Type::INT.asStatic();
    CHECK_EQUAL(true, t.isStatic());
    CHECK_EQUAL(false, Type::BOOL.isStatic());
}

TEST(PrintBasic){
    CHECK_EQUAL(" of type bool",
                Type::BOOL.predicate()
                );
    CHECK_EQUAL(" of type int",
                Type::INT.predicate()
                );
    CHECK_EQUAL(" of type void",
                Type::VOID.predicate()
                );
    CHECK_EQUAL(" of type record",
                Type::RECORD().predicate()
                );
}

TEST(PrintStatic){
    CHECK_EQUAL(" is static of type bool",
                Type::BOOL.asStatic().predicate()
                );
}

TEST(PrintArray){
    CHECK_EQUAL(" is array of type int",
                Type::INT.asArray(0).predicate()
                );
}

TEST(PrintStaticArray){
    CHECK_EQUAL(" is static array of type bool",
                Type::BOOL.asArray(0).asStatic().predicate()
                );
}
