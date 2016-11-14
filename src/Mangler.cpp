#include "Mangler.hpp"

using namespace std;

namespace{
    bool printQuotes(const Token* t){
        char c = t->text[0];
        if(t->token == ID || t->token == BOOLCONST)
            return true;
        if(c=='\'' || c=='\"')
            return false;
        if( (c >= 'a' && c <= 'z') ||  (c >= 'A' && c<= 'Z'))
            return false;
        return true;
    }

    void breakoutItems(const char* bisonMsg, vector<string>& items){
        int index = 0;
        const char* word = bisonMsg;
        while(*word != '\0'){
            word++;
            const char* end = word;
            while(*end != ' ' && *end != '\0')
                end++;
            index++;
            if(index >= 5 && index%2 == 0) {
                items.push_back(string(word, end-word));
            }
            word = end;
        }
    }

    map<string,string> setupSimplifyMap(){
        map<string,string> simplifyMap;
        simplifyMap["AND"]         = "and";
        simplifyMap["OR"]          = "or";
        simplifyMap["NOT"]         = "not";
        simplifyMap["NOTEQ"]       = "'!='";
        simplifyMap["MULASS"]      = "'*='";
        simplifyMap["INC"]         = "'++'";
        simplifyMap["ADDASS"]      = "'+='";
        simplifyMap["DEC"]         = "'--'";
        simplifyMap["SUBASS"]      = "'-='";
        simplifyMap["DIVASS"]      = "'/='";
        simplifyMap["LESSEQ"]      = "'<='";
        simplifyMap["EQ"]          = "'=='";
        simplifyMap["GRTEQ"]       = "'>='";
        simplifyMap["BOOL"]        = "bool";
        simplifyMap["BREAK"]       = "break";
        simplifyMap["CHAR"]        = "char";
        simplifyMap["ELSE"]        = "else";
        simplifyMap["IF"]          = "if";
        simplifyMap["IN"]          = "in";
        simplifyMap["INT"]         = "int";
        simplifyMap["RECORD"]      = "record";
        simplifyMap["RECTYPE"]     = "rectype";
        simplifyMap["RETURN"]      = "return";
        simplifyMap["STATIC"]      = "static";
        simplifyMap["WHILE"]       = "while";
        simplifyMap["BOOLCONST"]   = "Boolean constant";
        simplifyMap["NUMCONST"]    = "numeric constant";
        simplifyMap["ID"]          = "identifier";
        simplifyMap["CHARCONST"]   = "character constant";
        simplifyMap["STRINGCONST"] = "string constant";
        simplifyMap["$end"]        = "end of input";
        return simplifyMap;
    }
    map<string,string> simplifyMap = setupSimplifyMap();

    string simplify(string input){
        if(simplifyMap[input] != "") return simplifyMap[input];
        return input;
    }
}

std::string Mangler::mangleErrorString(const char* bisonMsg, const Token* lastToken){
    std::ostringstream tmp;
    tmp << "Syntax error, unexpected ";
    switch(lastToken->token){
        case ID:        tmp << "identifier "; break;
        case BOOLCONST: tmp << "Boolean constant "; break;
        case NUMCONST:  tmp << "numeric constant "; break;
        case CHARCONST: tmp << "character constant "; break;
    }
    if(printQuotes(lastToken)) tmp << "\'" << lastToken->text << "\'";
    else tmp << lastToken->text;

    //map and sort expected items
    vector<string> expected;
    breakoutItems(bisonMsg, expected);
    for(size_t i=0; i<expected.size(); i++){
        expected[i] = simplify(expected[i]);
    }
    sort(expected.begin(), expected.end());

    for(size_t i=0; i<expected.size(); i++){
        if(i == 0){
            tmp << ", expecting ";
        } else {
            tmp << " or ";
        }
        tmp << expected[i];
    }

    tmp << ".";
/*
    cerr << std::string(bisonMsg) << endl;
    cerr << tmp.str() << endl;
    cerr << endl;*/
    //return std::string(bisonMsg);
    //return tmp.str();
    return tmp.str();
}
