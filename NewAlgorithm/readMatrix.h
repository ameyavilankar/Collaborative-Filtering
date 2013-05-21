#ifndef READ_MATRIX_H
#define READ_MATRIX_H

// readMatrix.h version 1.0
// Author: Ameya Vilankar

#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <map>
#include <ctime>

// Used to split the string based on spaces
std::vector<double> split(const std::string& s);
int getRatingMatrix(const char* filename,std:: vector<std::vector<double> >& ratingMatrix);
template<class fwditer>fwditer random_unique(fwditer begin, fwditer end, size_t num_random);

#endif

