#ifndef TYPE_H
#define TYPE_H
#include <string>
#include <sstream>
#include <assert.h>

#include "TextUtils.hpp"

class Type{
private:
    cstr raw;
    int _array;
    bool _static, _func;
    Type(cstr str, int _array, bool _static):
        raw(str), _array(_array), _static(_static), _func(false) {}
    Type(cstr str, int _array, bool _static, bool _func):
        raw(str), _array(_array), _static(_static), _func(_func) {}
    static const int NON_ARRAY = -1;
public:
    static const Type INT;
    static const Type BOOL;
    static const Type VOID;
    static const Type CHAR;
    static const Type NONE;
    static Type RECORD(/*record pointer*/){ return Type("record", NON_ARRAY, false); }
    /** Return an array type of length `length` of this type */
    Type asArray(int length) const {
        assert(!isArray()); // no arrays of arrays in c-
        return Type(raw, length, _static);
    }
    /** Return an static type of this type */
    Type asStatic() const {
        return Type(raw, _array, true);
    }
    Type asFunc() const {
        return Type(raw, NON_ARRAY, false, true);
    }
    Type returnType() const {
        return Type(raw, NON_ARRAY, false, false);
    }
    /**
     * Get a version of this type representing its runtime meaning
     * For now, this just cleares the static field because a static int
     * is assignable to an int field etc.
     */
    Type runtime() const {
        return Type(raw, _array, false, _func);
    }
    bool isArray() const { return _array >= 0; }
    bool isStatic() const { return _static; }
    bool isFunction() const { return _func; }
    int arrayLength() const { return _array; }
    int size() const { return (isArray())? _array+1 : 1; }
    int offset() const { return (isArray())? 1 : 0; }
    const cstr rawString() const { return raw; }
    /**
     * Return a sentence predicate that qualifies a prefixed id name as this
     * type
     */
    std::string predicate() const {
        std::ostringstream oss;
        if(isArray()){
            oss << " is array";
        }
        oss << " " /*"of type " << raw*/;
        return oss.str();
    }
    std::string toString() const {
        std::ostringstream oss;
        if(isFunction()){
            oss << "Function returning ";
        }
        if(isStatic()){
            oss << "static ";
        }
        oss << raw;
        if(isArray()){
            oss << " array";
        }
        return oss.str();
    }
    std::string typeBox() const {
        std::ostringstream oss;
        oss << "[";
        if(this->rawString() == NONE.rawString()){
            oss<<"undefined type";
        } else {
            oss<<"type "<<this->rawString();
        }
        oss << "]";
        return oss.str();
    }
    /**
     * Detemine if two Type objects are semantically equal
     */
     bool operator==(const Type& n) const {
        return raw == n.raw &&
               _array == n._array &&
               _static == n._static;
    }
    bool operator!=(const Type& n) const {
        return !(*this == n);
    }
};
std::ostream& operator<<(std::ostream&, const Type&);
std::ostream& operator<<(std::ostream&, const Type*);

#endif
