#include "UnitTest++/UnitTest++.h"
#include "Token.hpp"
#include <string>
#include <sstream>

TEST(TokenConstructor){
    Token t(0, 0, "");
}

TEST(TokenEquals){
    CHECK(Token(0,1,"A") == Token(0,1,"A"));
    CHECK(Token(0,5,"B") == Token(0,5,"B"));
    CHECK(!(Token(0,5,"C") == Token(0,5,"B")));
    CHECK(!(Token(0,1,"C") == Token(0,2,"C")));
}

TEST(TokenStringFormat){
    CHECK_EQUAL("Line 46 Token: *", Token(0,46,"*").toString());
    CHECK_EQUAL("Line 74 Token: -", Token(0,74,"-").toString());
    CHECK_EQUAL("Line 49 Token: LESSEQ", Token(0,49,"LESSEQ").toString());
    CHECK_EQUAL("Line 60 Token: RECORD", Token(0,60,"RECORD").toString());
}

TEST(CharconstConstructor){
    CharConst c(0, 46, "a");
}

TEST(CharcostEquals){
    CHECK(CharConst(0,1,"A") == CharConst(0,1,"A"));
    CHECK(CharConst(0,5,"\\n") == CharConst(0,5,"\\n"));
    CHECK(!(CharConst(0,5,"C") == CharConst(0,5,"B")));
    CHECK(!(CharConst(0,1,"\\0") == CharConst(0,2,"\\0")));
}

TEST(CharConstStringFormat){
    CHECK_EQUAL("Line 2 Token: CHARCONST Value: \'x\'  Input: \'x\'",
                CharConst(0, 2, "\'x\'").toString());
    CHECK_EQUAL("Line 5 Token: CHARCONST Value: \'\"\'  Input: \'\\\"\'",
                CharConst(0, 5, "\'\\\"\'").toString());

    std::ostringstream oss;
    oss << "Line 7 Token: CHARCONST Value: \'" << '\0' << "\'  Input: \'\\0\'";
    CHECK_EQUAL(oss.str(),
                CharConst(0, 7, "\'\\0\'").toString());
}

TEST(IdConstructor){
    IdToken IdToken(0, 10, "dogs");
}

TEST(IdTokenEquals){
    CHECK(IdToken(0,1,"A") == IdToken(0,1,"A"));
    CHECK(IdToken(0,5,"\\n") == IdToken(0,5,"\\n"));
    CHECK(!(IdToken(0,5,"C") == IdToken(0,5,"B")));
    CHECK(!(IdToken(0,1,"\\0") == IdToken(0,2,"\\0")));
}

TEST(IdTokenStringFormat){
    CHECK_EQUAL("Line 25 Token: ID Value: cats",
                IdToken(0, 25, "cats").toString());
    CHECK_EQUAL("Line 90 Token: ID Value: redrum",
                IdToken(0, 90, "redrum").toString());
    CHECK_EQUAL("Line 89 Token: ID Value: AllWorkAndNoPlayMakesJackADullBoyAllWorkAndNoPlayMakesJackADullBoyAllWorkAndNoPlayMakesJackADullBoy",
                IdToken(0, 89, "AllWorkAndNoPlayMakesJackADullBoyAllWorkAndNoPlayMakesJackADullBoyAllWorkAndNoPlayMakesJackADullBoy").toString());
}

TEST(NumConstConstructor){
    NumConst nc(0, 44, "10");
}

TEST(NumConstEquals){
    CHECK(NumConst(0,1,"123") == NumConst(0,1,"123"));
    CHECK(NumConst(0,5,"456") == NumConst(0,5,"456"));
    CHECK(!(NumConst(0,5,"123") == NumConst(0,5,"456")));
    CHECK(!(NumConst(0,1,"0") == NumConst(0,2,"0")));
}

TEST(NumConstFormat){
    CHECK_EQUAL("Line 77 Token: NUMCONST Value: 7  Input: 007",
                NumConst(0, 77, "007").toString());
    CHECK_EQUAL("Line 76 Token: NUMCONST Value: 0  Input: 0",
                NumConst(0, 76, "0").toString());
    CHECK_EQUAL("Line 81 Token: NUMCONST Value: -1241815933  Input: 14159265358979",
                NumConst(0, 81, "14159265358979").toString());
    CHECK_EQUAL("Line 83 Token: NUMCONST Value: 14  Input: 14",
                NumConst(0, 83, "14").toString());
}

TEST(NumConstLiteralValue){
    CHECK_EQUAL(7, NumConst(0, 77, "007").value);
    CHECK_EQUAL(0, NumConst(0, 76, "0").value);
    CHECK_EQUAL(-1241815933, NumConst(0, 81, "14159265358979").value);
    CHECK_EQUAL(14, NumConst(0, 83, "14").value);
}

TEST(BoolConstConstructor){
    BoolConst bc(0, 77, "true");
}

TEST(BoolConstEquals){
    CHECK(BoolConst(0,1,"true") == BoolConst(0,1,"true"));
    CHECK(BoolConst(0,5,"false") == BoolConst(0,5,"false"));
    CHECK(!(BoolConst(0,5,"true") == BoolConst(0,5,"false")));
    CHECK(!(BoolConst(0,1,"false") == BoolConst(0,2,"false")));
}

TEST(BoolConstFormat){
    CHECK_EQUAL("Line 43 Token: BOOLCONST Value: 1  Input: true",
        BoolConst(0,43,"true").toString());
    CHECK_EQUAL("Line 44 Token: BOOLCONST Value: 0  Input: false",
        BoolConst(0,44,"false").toString());
}

TEST(InvalidConstructor){
    Invalid iv(0,0,"@");
}

TEST(InvalidEquals){
    CHECK(Invalid(0,1,"true") == Invalid(0,1,"true"));
    CHECK(Invalid(0,5,"false") == Invalid(0,5,"false"));
    CHECK(!(Invalid(0,5,"true") == Invalid(0,5,"false")));
    CHECK(!(Invalid(0,1,"false") == Invalid(0,2,"false")));
}

TEST(InvalidFormat){
    CHECK_EQUAL("ERROR(38): Invalid or misplaced input character: \"_\"",
                Invalid(0,38,"_").toString());
    CHECK_EQUAL("ERROR(31): Invalid or misplaced input character: \"@\"",
                Invalid(0,31,"@").toString());
    CHECK_EQUAL("ERROR(9): Invalid or misplaced input character: \"\'\"",
                Invalid(0,9,"\'").toString());
}

TEST(SubclassInequalityInvariant){
    CHECK(!(BoolConst(0,1,"true")==IdToken(0,1,"true")));
    CHECK(!(Token(0,1,"12")==NumConst(0,1,"true")));
    CHECK(!(Invalid(0,1,"@")==CharConst(0,1,"t")));
}
