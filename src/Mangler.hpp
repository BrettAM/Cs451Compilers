#ifndef MANGLER_H
#define MANGLER_H

#include <iostream>
#include <sstream>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include "TextUtils.hpp"
#include "Token.hpp"

namespace Mangler{
    std::string mangleErrorString(const char* bisonMsg, const Token* lastToken);
}

#endif
