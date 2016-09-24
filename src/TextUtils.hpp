#ifndef _H
#define _H

#include <string>
#include <sstream>
typedef const char * cstr;

namespace TextUtils{
    /**
     * Returns the first character in the unescaped interpretation of `s`
     * '\n' and '\0' are special escaped sequences, '\*' return *
     */
    char unescape(cstr s);
}

#endif
