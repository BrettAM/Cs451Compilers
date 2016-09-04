#include "ParseDriver.hpp"

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern int yyparse();
extern int yylineno;
extern FILE* yyin;
extern YY_BUFFER_STATE yy_scan_string(const char * str);
extern YY_BUFFER_STATE yy_create_buffer ( FILE *file, int size );
extern void yy_switch_to_buffer ( YY_BUFFER_STATE new_buffer );
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

using namespace ParseDriver;

namespace {
    std::vector<Token*> results;

    void setup(){
        results = std::vector<Token*>();
    }

    Result teardown(){
        return results;
    }
}

void ParseDriver::pushToken(Token* t){
    results.push_back(t);
}

void ParseDriver::pushError(const char * msg){
    std::cerr << "Error: " << msg << std::endl;
}

std::vector<Token*> ParseDriver::run(const char* str){
    YY_BUFFER_STATE buf = yy_scan_string(str);

    setup();

    yy_switch_to_buffer(buf);
    yylineno = 1;
    yyparse();
    yy_delete_buffer(buf);

    return teardown();
}

std::vector<Token*> ParseDriver::run(FILE* f){
    //YY_BUFFER_STATE buf = yy_create_buffer(f,YY_BUF_SIZE);

    setup();

    //yy_switch_to_buffer(buf);
    yylineno = 1;
    yyin = f;
    yyparse();
    //yy_delete_buffer(buf);

    return teardown();
}
