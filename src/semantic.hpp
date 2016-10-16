#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <vector>
#include "Type.hpp"
#include "Error.hpp"
#include "AST.hpp"
#include "SymbolTable.hpp"

namespace Semantics{
    typedef struct ChkResult {
        ChkResult(Type t): passed(true), error(NULL), result(t) {}
        ChkResult(Error* e): passed(false), error(e), result(Type::NONE) {}
        bool passed;
        Error* error;
        Type result;
    } ChkResult;
    /**
     * Check if the code at `call` is a valid invocation of `function`
     * return NULL if its correct, or a new Error* if not
     */
    ChkResult checkCall(AST::Node* call, AST::Node* function);
    /**
     * Check if an invocation of a given operator is valid on the given nodes
     */
    ChkResult checkOperands(AST::Node* op);
    /**
     * Analyse an AST and return a list of detected errors
     * This will result in the `type` field at each node being filled in where
     *    possible
     */
    std::vector<Error*> analyze(AST::Node* root);
}

#endif
