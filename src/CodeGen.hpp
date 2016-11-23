#ifndef CODEGEN_H
#define CODEGEN_H

#include <vector>
#include "Type.hpp"
#include "Error.hpp"
#include "AST.hpp"
#include "SymbolTable.hpp"
#include "ParseDefines.hpp"

namespace CodeGen{
    /**
     * Traverse the input tree, upating the tree node's
     *   `location` field with a calculated runtime location
     */
    void calculateLocations(AST::Node* tree);
}

#endif
