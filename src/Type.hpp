#ifndef TYPE_H
#define TYPE_H
#include <string>
#include <sstream>
#include <assert.h>

#include "TextUtils.hpp"

class Type{
private:
    cstr raw;
    bool _array, _static, _func;
    Type(cstr str, bool _array, bool _static):
        raw(str), _array(_array), _static(_static), _func(false) {}
    Type(cstr str, bool _array, bool _static, bool _func):
        raw(str), _array(_array), _static(_static), _func(_func) {}
public:
    static const Type INT;
    static const Type BOOL;
    static const Type VOID;
    static const Type CHAR;
    static const Type NONE;
    static Type RECORD(/*record pointer*/){ return Type("record", false, false); }
    /** Return an array type of length `length` of this type */
    Type asArray(int length) const {
        assert(!_array); // no arrays of arrays in c-
        return Type(raw, true, _static);
    }
    /** Return an static type of this type */
    Type asStatic() const {
        return Type(raw, _array, true);
    }
    Type asFunc() const {
        return Type(raw, false, false, true);
    }
    Type returnType() const {
        if(_func){
            return Type(raw, false, false, false);
        } else {
            return NONE;
        }
    }
    bool isArray() const { return _array; }
    bool isStatic() const { return _static; }
    bool isFunction() const { return _func; }
    const cstr rawString() const { return raw; }
    /**
     * Return a sentence predicate that qualifies a prefixed id name as this
     * type
     */
    std::string toString() const {
        std::ostringstream oss;
        if(_array || _static){
            oss << " is";
            if(_static) oss << " static";
            if(_array) oss << " array";
        }
        oss << " of type " << raw;
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
};
std::ostream& operator<<(std::ostream&, const Type&);
std::ostream& operator<<(std::ostream&, const Type*);

#endif
