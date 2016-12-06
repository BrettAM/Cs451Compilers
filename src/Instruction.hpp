#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "Location.hpp"
#include "TextUtils.hpp"
#include "Registers.hpp"

#include <iostream>
#include <sstream>

class Instruction{
private:
    Location address;
    std::string operation;
    std::string cmt;
    int result;
    enum OpType {RegReg, RegMem, JustOp, Comment} optype;
    int op1, op2;
    Location memory;
    Instruction(std::string comment):
        cmt(comment), optype(Comment) {}
    Instruction(std::string op, std::string comment):
        operation(op), cmt(comment),
        optype(JustOp) {}
    Instruction(std::string op, std::string comment, int result, int op1, int op2):
        operation(op), cmt(comment), result(result),
        optype(RegReg), op1(op1), op2(op2) {}
    Instruction(std::string op, std::string comment, int result, Location memory):
        operation(op), cmt(comment), result(result),
        optype(RegMem), memory(memory) {}
public:
    /**
     * Sets this Instructions line number. Must be set before emitting the
     *   instruction; can only be set once.
     */
    void setLocation(int lineno);
    const Location* getLocation();
    /**
     * Format this instruction for interpretation
     */
    void emit(std::ostream& output) const;
    /**
     * Return the `emit` text as a string
     */
    std::string toString() const;
    bool isComment() const { return optype == Comment; }
    enum Op {
        Add, Sub, Mul, Div, And, Or, Xor, Not, Swap, Random,
        LessThan, LessEqual, Equal, NotEqual, GreaterEqual, Greater,
    };
    static Instruction* comment(std::string label, std::string text = "");
    static Instruction* halt(std::string cmt = "");
    static Instruction* nop(std::string cmt = "");
    static Instruction* ASM(std::string instr, std::string cmt = "");
    static Instruction* alu(Op operation, int r1, int r2, int r3, std::string cmt = "");
    static Instruction* move(int r, int source, std::string cmt = "");
    static Instruction* loadConst(int r, int const, std::string cmt = "");
    static Instruction* addConst(int r, int source ,int const, std::string cmt = "");
    static Instruction* load(int r, Location l, std::string cmt = "");
    static Instruction* store(int r, Location l, std::string cmt = "");
    static Instruction* jmp(Location l, std::string cmt = "");
    static Instruction* relJmp(Location l, std::string cmt);
    static Instruction* jmpNotZero(int testReg, Location l, std::string cmt = "");
    static Instruction* jmpZero(int testReg, Location l, std::string cmt = "");
};

#endif
