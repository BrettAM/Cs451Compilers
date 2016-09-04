#include "Token.hpp"

using namespace std;

ostream& operator<<(ostream& os, const Token& t){
    os << t.toString();
    return os;
}

ostream& operator<<(ostream& os, const Token* t){
    os << t->toString();
    return os;
}
