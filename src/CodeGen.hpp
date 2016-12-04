#ifndef CODEGEN_H
#define CODEGEN_H

#include <vector>
#include "Type.hpp"
#include "Error.hpp"
#include "AST.hpp"
#include "SymbolTable.hpp"
#include "ParseDefines.hpp"

namespace CodeGen{
    void generate(AST::Node* tree, FILE* output);
}

#endif
