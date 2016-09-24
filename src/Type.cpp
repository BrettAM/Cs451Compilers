#include "Type.hpp"

using namespace std;

const Type Type::INT("int", false, false);
const Type Type::BOOL("bool", false, false);
const Type Type::VOID("void", false, false);
const Type Type::CHAR("char", false, false);

ostream& operator<<(ostream& os, const Type& t){
    os << t.toString();
    return os;
}

ostream& operator<<(ostream& os, const Type* t){
    os << t->toString();
    return os;
}
