#include "Type.hpp"

using namespace std;

const Type Type::INT("int", NON_ARRAY, false);
const Type Type::BOOL("bool", NON_ARRAY, false);
const Type Type::VOID("void", NON_ARRAY, false);
const Type Type::CHAR("char", NON_ARRAY, false);
const Type Type::NONE("undefined", NON_ARRAY, false);

ostream& operator<<(ostream& os, const Type& t){
    os << t.toString();
    return os;
}

ostream& operator<<(ostream& os, const Type* t){
    os << t->toString();
    return os;
}
