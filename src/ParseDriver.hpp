#ifndef PARSEDRIVER_H
#define PARSEDRIVER_H

#include <vector>
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <map>
#include "Token.hpp"
#include "AST.hpp"
#include "Error.hpp"

namespace ParseDriver {
    class Result {
    private:
        std::vector<const Token*>* tokens;
        AST::Node* AST;
        std::vector<Error*>* errors;
    public:
        Result(std::vector<const Token*>* tokens, AST::Node* AST, std::vector<Error*>* errors):
            tokens(tokens), AST(AST), errors(errors) {}
        const std::vector<const Token*>* getTokens() const { return tokens; }
        AST::Node* getAST() const { return AST; }
        bool getErrorFlag() const {
            for(size_t i=0; i<errors->size(); i++){
                if(!errors->at(i)->isWarning()) return true;
            }
            return false;
        }
        std::vector<Error*>* getErrors() const { return errors; }
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
    void parseError(const char *);
    void pushError(Error *);
    /**
     * Interface for flex to add a token pointer to the results
     */
    int pushToken(const Token* t);
    /**
     * Interface for bison to return the final AST
     */
    void rootAST(AST::Node * AST);
    /**
     * Add a record to the global id namespace
     */
    void pushGlobal(const Token* record);
    /**
     * Returns true if an id name matches a previously defined record
     */
    bool isRecord(cstr idname);

    ///////// functions below for use by the compiler ///////////
    /**
     * POD class containing the specification for a code source buffer
     */
    class Source{
    public:
        static const Source IOLibrary;
        union { const char* str; FILE* file; };
        enum Type { STRING, DISK } type;
        int lineStart;
        Source(const char* str):
            str(str), type(STRING), lineStart(1) {}
        Source(const char* str, int lineno):
            str(str), type(STRING), lineStart(lineno) {}
        Source(FILE* f):
            file(f), type(DISK), lineStart(1) {}
        Source(FILE* f, int lineno):
            file(f), type(DISK), lineStart(lineno) {}
    };
    /**
     * Run a flex/bison pass over Source or sequence of Sources
     */
    Result run(Source source);
    Result run(std::vector<Source> sources);
    /**
     * Enable debugging details in flex and bison
     */
     void enableDebug();
}

#endif
