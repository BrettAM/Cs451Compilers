#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <vector>
#include "Type.hpp"
#include "Error.hpp"
#include "AST.hpp"
#include "SymbolTable.hpp"
#include "ParseDefines.hpp"

namespace Semantics{
    /**
     * Check if the code at `call` is a valid invocation of `function`
     * Push any detected errors into `errors`
     * return the calls result type
     */
    Type checkCall(AST::Node* call,
                   AST::Node* function,
                   std::vector<Error*>& errors);
    /**
     * Check if an invocation of a given operator is valid on the given nodes
     * pushes errors to into `errors`
     * returns the result type
     */
    Type checkOperands(AST::Node* op, std::vector<Error*>& errors);
    /**
     * Analyse an AST and return a list of detected errors
     * This will result in the `type` field at each node being filled in where
     *    possible
     */
    std::vector<Error*> analyze(AST::Node* root);
}

#endif
