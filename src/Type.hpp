#ifndef TYPE_H
#define TYPE_H
#include <string>
#include <sstream>
#include <assert.h>

#include "TextUtils.hpp"

class Type{
private:
    cstr raw;
    bool _array, _static;
    Type(cstr str, bool _array, bool _static):
        raw(str), _array(_array), _static(_static) {}
public:
    static const Type INT;
    static const Type BOOL;
    static const Type VOID;
    static const Type CHAR;
    static const Type NONE;
    static Type RECORD(/*record pointer*/){ return Type("record", false, false); }
    /** Return an array type of length `length` of this type */
    Type mkArray(int length) const {
        assert(!_array); // no arrays of arrays in c-
        return Type(raw, true, _static);
    }
    /** Return an static type of this type */
    Type mkStatic() const {
        return Type(raw, _array, true);
    }
    bool isArray() const { return _array; }
    bool isStatic() const { return _static; }
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
