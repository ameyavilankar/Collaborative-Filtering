#ifndef MAIN_H
#define MAIN_H

//main.h -version 1.0
// Author: Ameya Vilankar

#include "readMatrix.h"
#include "distances.h"
#include "constants.h"
#include <sstream>

template<class T> std::string to_string(T n)
{
    std::ostringstream os;
    os << n;
    return os.str();
}

#endif
