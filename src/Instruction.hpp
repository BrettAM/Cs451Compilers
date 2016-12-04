#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "Location.hpp"
#include "TextUtils.hpp"

#include <iostream>
#include <sstream>

enum Registers{
    GLOBALFRM = 0,
    LOCALFRM = 1,
    RETURNVAL = 2,
    ACC1 = 3,
    ACC2 = 4,
    ACC3 = 5,
    ZERO = 6,
    PC = 7
};

class Instruction{
private:
    Location address;
    cstr operation;
    cstr comment;
    int result;
    enum OpType {RegReg, RegMem, JustOp} optype;
    int op1, op2;
    Location memory;
    Instruction(cstr op, cstr comment):
        operation(op), comment(comment),
        optype(JustOp) {}
    Instruction(cstr op, cstr comment, int result, int op1, int op2):
        operation(op), comment(comment), result(result),
        optype(RegReg), op1(op1), op2(op2) {}
    Instruction(cstr op, cstr comment, int result, Location memory):
        operation(op), comment(comment), result(result),
        optype(RegMem), memory(memory) {}
public:
    /**
     * Sets this Instructions line number. Must be set before emitting the
     *   instruction; can only be set once.
     */
    void setLocation(int lineno);
    /**
     * Format this instruction for interpretation
     */
    void emit(std::ostream& output) const;
    /**
     * Return the `emit` text as a string
     */
    std::string toString() const;
    enum Op {
        Add, Sub, Mul, Div, And, Or, Xor, Not, Swap, Random,
        LessThan, LessEqual, Equal, NotEqual, GreaterEqual, Greater,
    };
    static Instruction halt(cstr cmt = "");
    static Instruction nop(cstr cmt = "");
    static Instruction alu(Op operation, int r1, int r2, int r3, cstr cmt = "");
    static Instruction move(int r, int source, cstr cmt = "");
    static Instruction loadConst(int r, int const, cstr cmt = "");
    static Instruction addConst(int r, int source ,int const, cstr cmt = "");
    static Instruction load(int r, Location l, cstr cmt = "");
    static Instruction store(int r, Location l, cstr cmt = "");
    static Instruction jmp(Location l, cstr cmt = "");
    static Instruction relJmp(Location l, cstr cmt);
    static Instruction jmpNotZero(int testReg, Location l, cstr cmt = "");
    static Instruction jmpZero(int testReg, Location l, cstr cmt = "");
};

#endif
