#ifndef PARSEDRIVER_H
#define PARSEDRIVER_H

#include <vector>
#include <stdio.h>
#include <iostream>
#include "Token.hpp"

namespace ParseDriver {
    typedef std::vector<Token*> Result;

    // Function below interface the driver and flex/bison

    /**
     *
     */
    void pushToken(Token*);

    /**
     *
     */
    void pushError(const char *);

    // Functions below for use by the project

    Result run(const char* str);
}

#endif
