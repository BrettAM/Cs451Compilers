#ifndef CODEGEN_H
#define CODEGEN_H

#include <vector>
#include <set>
#include <iostream>
#include <sstream>
#include "Type.hpp"
#include "Error.hpp"
#include "AST.hpp"
#include "SymbolTable.hpp"
#include "ParseDefines.hpp"
#include "Location.hpp"
#include "Instruction.hpp"
#include "Registers.hpp"

namespace CodeGen{
    void generate(AST::Node* tree, std::ostream& output);
}

#endif
