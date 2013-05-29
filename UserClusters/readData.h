#ifndef READ_DATA_H
#define READ_DATA_H

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
#include <wctype.h>
#include <limits>

// Used to split the string based on spaces
std::vector<double> split(const std::string& s);
std::vector<std::vector<double> > getRatingMatrix(const char* filename, std::map<long, int>& userMap, std::map<long, int>& movieMap);
double squareDistance(const std::vector<double>& a, const std::vector<double>& b);
int getUserToClusterMap(const char* filename, std::map<long, int>& userToClusterMap, std::map<int, std::vector<long> >& clusterToUserMap);

#endif

