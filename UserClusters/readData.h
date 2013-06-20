#ifndef READ_DATA_H
#define READ_DATA_H

// readMatrix.h version 1.0
// Author: Ameya Vilankar
#include "movie.h"
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

typedef bool (*skip)(char c);

// Function used to determine if the current character is a colon or not
inline bool isColon(char c)
{
	return c == ':';
}

// Function used to determine if the current character is a colon or not
inline bool isSpace(char c)
{
	return iswspace(c);
}

// Used to split the string based on spaces
std::vector<double> split(const std::string& s, skip isSkip);
int getRatingMatrix(const char* filename, std::map<long, int>& userMap, std::map<long, int>& movieMap, std::vector<std::vector<double> >& ratingMatrix);
double squareDistance(const std::vector<double>& a, const std::vector<double>& b);
int getUserToClusterMap(const char* filename, std::map<long, int>& userToClusterMap, std::map<int, std::vector<long> >& clusterToUserMap);

int getMovieMap(const char* filename, std::map<long, PipeFish::Movie>& movieMap);
std::vector<std::string> getMovieIdName(const std::string& s);

#endif

