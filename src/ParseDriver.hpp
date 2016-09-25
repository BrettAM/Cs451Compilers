#ifndef PARSEDRIVER_H
#define PARSEDRIVER_H

#include <vector>
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include "Token.hpp"
#include "AST.hpp"

namespace ParseDriver {
    class Result {
    private:
        std::vector<const Token*>* tokens;
        AST::Node* AST;
    public:
        Result(std::vector<const Token*>* tokens, AST::Node* AST):
            tokens(tokens), AST(AST) {}
        const std::vector<const Token*>* getTokens() const { return tokens; }
        AST::Node* getAST() const { return AST; }
        /**
         * Free all the memory associated with the result. All references
         * to the original tokens and AST become invalid.
         */
        void cleanup();
    };

    ///////// function below for use by flex/bison //////////
    /**
     * Interface to log an error from flex/bison
     */
    void pushError(const char *);
    /**
     * Interface for flex to add a token pointer to the results
     */
    int pushToken(const Token* t);

    ///////// functions below for use by the compiler ///////////
    /**
     * Run a flex/bison pass over an input string
     */
    Result run(const char* str);
    /**
     * Run a flex/bison pass over the contents of a file
     */
    Result run(FILE* f);
}

#endif
