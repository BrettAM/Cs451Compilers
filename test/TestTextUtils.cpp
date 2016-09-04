#include "UnitTest++/UnitTest++.h"
#include "TextUtils.hpp"

using namespace TextUtils;

TEST(UnescapeSingleChar){
    CHECK_EQUAL('a', unescape("a"));
    CHECK_EQUAL('A', unescape("A"));
    CHECK_EQUAL('7', unescape("7"));
    CHECK_EQUAL('#', unescape("#"));
}

TEST(UnescapeLongInput){
    CHECK_EQUAL('a', unescape("abcd"));
    CHECK_EQUAL('A', unescape("ABCD"));
    CHECK_EQUAL('7', unescape("7890"));
    CHECK_EQUAL('#', unescape("#$&{"));
}

TEST(UnescapeNewline){
    CHECK_EQUAL('\n', unescape("\\n"));
}

TEST(UnescapeNull){
    CHECK_EQUAL('\0', unescape("\\0"));
}

TEST(UnescapeSingleQuote){
    CHECK_EQUAL('\'', unescape("\\\'"));
}

TEST(UnescapeQuote){
    CHECK_EQUAL('\"', unescape("\\\""));
}

TEST(UnescapeEmpty){
    CHECK_EQUAL('\0', unescape(""));
}

TEST(UnescapeSingleEscapeChar){
    CHECK_EQUAL('\\', unescape("\\"));
}

TEST(UnescapeEscapedNormalChar){
    CHECK_EQUAL('a', unescape("\\a"));
    CHECK_EQUAL('A', unescape("\\A"));
    CHECK_EQUAL('7', unescape("\\7"));
    CHECK_EQUAL('#', unescape("\\#"));
}
