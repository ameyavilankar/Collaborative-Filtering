#ifndef TO_STRING_H
#define TO_STRING_H

// to_string.h - v1.0 
// Author: Ameya Vilankar

#include <sstream>
#include <string>

template<class T> std::string to_string(T n)
{
    std::ostringstream os;
    os << n;
    return os.str();
}

#endif