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
extern void yypush_buffer_state(YY_BUFFER_STATE buffer);
extern void yypop_buffer_state();

// Default file buffer size for flex is 16k
size_t YY_BUF_SIZE = 0x4000;

using namespace ParseDriver;
using namespace AST;
using namespace std;

namespace { // file local namespace
    std::vector<const Token*>* tokenList;
    map<string,const Token *> globs;
    AST::Node* ASTroot;
    std::vector<Error*>* errors;

    void setup(){
        tokenList = new std::vector<const Token*>();
        globs = map<string,const Token *>();
        ASTroot = NULL;
        errors = new std::vector<Error*>();
    }

    void teardown(){
        tokenList = NULL;
        globs.clear();
        ASTroot = NULL;
        errors = NULL;
    }
}

int ParseDriver::pushToken(const Token* t){
    tokenList->push_back(t);
    return t->token;
}

void ParseDriver::rootAST(Node * AST){
    ASTroot = AST;
}

void ParseDriver::parseError(const char * msg){
    errors->push_back(
        Error::Err(yylineno,
            Mangler::mangleErrorString(msg, tokenList->back())));
}

void ParseDriver::pushError(Error * error){
    errors->push_back(error);
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

Result ParseDriver::run(Source source){
    return ParseDriver::run(listof<Source>() << source);
}

Result ParseDriver::run(std::vector<Source> sources){
    setup();

    // Each successfully compiled source produces an abstract syntax tree
    //   rooted with a sibling node. This code iterates through sources,
    //   taking all the root sibling's children and adding them to a new
    //   sibling list encompasing the entire program.

    listof<Node*> fullChildList;
    for(size_t i=0; i<sources.size(); i++){
        // parse the next source
        Source& source = sources[i];
        yylineno = source.lineStart;

        YY_BUFFER_STATE buf =
            (source.type == Source::DISK)
                ? yy_create_buffer(source.file, YY_BUF_SIZE)
                : yy_scan_string(source.str);

        yy_switch_to_buffer(buf);
        yyparse();
        yy_delete_buffer(buf);

        // log results
        if(ASTroot != NULL){
            fullChildList.addAll(ASTroot->viewChildren());
            delete ASTroot;
            ASTroot = NULL;
        } else {
            break; // syntax error
        }
    }

    Node* root = Siblings(fullChildList);
    Result result = Result(tokenList, root, errors);

    teardown();
    return result;
}

void ParseDriver::Result::cleanup(){
    // free all the tokens
    for(std::vector<const Token*>::iterator itr = tokens->begin();
            itr!= tokens->end();
            ++itr){
        delete (*itr);
    }
    delete tokens;
    tokens = NULL;

    // free error pointers
    for(std::vector<Error*>::iterator itr = errors->begin();
            itr != errors->end();
            ++itr){
        delete (*itr);
    }
    delete errors;
    errors = NULL;

    // free all the tree nodes
    if(AST != NULL) {
        AST->deleteTree();
        AST = NULL;
    }
}

/**
 * A list of prototypes for builtin IO calls in c-
 * They should be listed without newlines so that they all appear
 *   as being defined on line -1
 */
const Source ParseDriver::Source::IOLibrary(
    "int input();"
    "output(int *dummy*);"
    "bool inputb();"
    "outputb(bool *dummy*);"
    "char inputc();"
    "outputc(char *dummy*);"
    "outnl();"
    ,-1
);
