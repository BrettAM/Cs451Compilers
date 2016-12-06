#include "UnitTest++/UnitTest++.h"
#include "Location.hpp"
#include "Instruction.hpp"
#include <string>
#include <sstream>
#include <exception>
using namespace std;

TEST(printNOP){
    auto ins = Instruction::nop();
    ins->setLocation(0);
    CHECK_EQUAL("0:\tNOP\t\t\n", ins->toString());
    delete ins;
}

TEST(printNOPcomment){
    auto ins = Instruction::nop("Comment");
    ins->setLocation(10);
    CHECK_EQUAL("10:\tNOP\t\tComment\n", ins->toString());
    delete ins;
}

TEST(printRR){
    auto ins = Instruction::alu(Instruction::Add, 1, 2, 3);
    ins->setLocation(0);
    CHECK_EQUAL("0:\tADD\t1,2,3\t\n", ins->toString());
    delete ins;
}

TEST(printRRcomment){
    auto ins = Instruction::alu(Instruction::Div, 4, 5, 6, "Comment");
    ins->setLocation(10);
    CHECK_EQUAL("10:\tDIV\t4,5,6\tComment\n", ins->toString());
    delete ins;
}

TEST(printRM){
    Location mem(MemoryRef::Data(15,7));
    auto ins = Instruction::load(3, mem);
    ins->setLocation(0);
    CHECK_EQUAL("0:\tLD \t3,15(7)\t\n", ins->toString());
    delete ins;
}

TEST(printRMcomment){
    Location mem(MemoryRef::Data(-30,6));
    auto ins = Instruction::store(3, mem, "Comment");
    ins->setLocation(10);
    CHECK_EQUAL("10:\tST \t3,-30(6)\tComment\n", ins->toString());
    delete ins;
}
