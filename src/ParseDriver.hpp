#ifndef PARSEDRIVER_H
#define PARSEDRIVER_H

#include <vector>
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include "Token.hpp"

namespace ParseDriver {
    typedef std::vector<Token*> Result;

    // Function below interface the driver and flex/bison

    /**
     *
     */
    void pushError(const char *);
    /**
     *
     */
    std::vector<Token*>* getResVec();

    // Functions below for use by the project

    Result run(const char* str);
    Result run(FILE* f);
}

#endif
