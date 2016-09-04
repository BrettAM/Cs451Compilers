#ifndef _H
#define _H

#include <string>
typedef const char * cstr;

namespace TextUtils{
    /**
     *
     */
    char unescape(cstr s);
    /**
     *
     */
    std::string strip(cstr s);
}

#endif
