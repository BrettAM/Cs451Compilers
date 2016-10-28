#include "ParseDriver.hpp"

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern int yyparse();
extern int yylineno;
extern int yydebug;
extern FILE* yyin;
extern YY_BUFFER_STATE yy_scan_string(const char * str);
extern YY_BUFFER_STATE yy_create_buffer ( FILE *file, int size );
extern void yy_switch_to_buffer ( YY_BUFFER_STATE new_buffer );
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

using namespace ParseDriver;
using namespace AST;
using namespace std;

void ParseDriver::Result::cleanup(){
    // free all the tokens
    for(std::vector<const Token*>::iterator itr = tokens->begin();
            itr!= tokens->end();
            ++itr){
        delete (*itr);
    }
    delete tokens;
    tokens = NULL;

    // free error pointer
    if(error != NULL){
        delete error;
        error = NULL;
    }

    // free all the tree nodes
    if(AST != NULL) {
        AST->deleteTree();
        AST = NULL;
    }
}

namespace { // file local namespace
    std::vector<const Token*>* tokenList;
    AST::Node* ASTroot;
    map<string,const Token *> globs;
    Error* error;

    void setup(){
        tokenList = new std::vector<const Token*>();
        globs = map<string,const Token *>();
        ASTroot = NULL;
        error = NULL;
    }

    Result teardown(){
        Result result = Result(
                tokenList,
                (ASTroot!=NULL) ? ASTroot : Siblings(listof<Node*>()),
                error
            );
        // ownership of these pointers passed to result;
        tokenList = NULL;
        ASTroot = NULL;
        error = NULL;
        return result;
    }
}

int ParseDriver::pushToken(const Token* t){
    tokenList->push_back(t);
    return t->token;
}

void ParseDriver::rootAST(Node * AST){
    ASTroot = AST;
}

void ParseDriver::pushError(const char * msg){
    std::ostringstream oss;
    oss << msg << " on line " << yylineno;
    error = new Error(Error::SYNTAX, oss.str());
}

void ParseDriver::pushGlobal(const Token* record){
    globs[string(record->text)]=record;
}

bool ParseDriver::isRecord(cstr idname){
    return (globs[string(idname)] != NULL);
}

void ParseDriver::enableDebug(){
    yydebug = 1;
}

Result ParseDriver::run(const char* str){
    return ParseDriver::run(str, 1);
}
Result ParseDriver::run(const char* str, int startLineNumber){
    YY_BUFFER_STATE buf = yy_scan_string(str);

    setup();

    yy_switch_to_buffer(buf);
    yylineno = startLineNumber;
    yyparse();
    yy_delete_buffer(buf);

    return teardown();
}

Result ParseDriver::run(FILE* f){
    return ParseDriver::run(f, 1);
}
Result ParseDriver::run(FILE* f, int startLineNumber){
    setup();

    yylineno = startLineNumber;
    yyin = f;
    yyparse();

    return teardown();
}
