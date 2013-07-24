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
#include <wctype.h>

// Used to split the string based on spaces
// Function used to determine if the current character is a colon or not
inline bool isSpace(char c)
{
	return iswspace(c);
}

// Convert the string to its double equivalent
inline double getDouble(std::string toConvert)
{
	return atof(toConvert.c_str());
}

// Funciton used to split the sentence into its individual parts
std::vector<double> split(const std::string& s);

// Overloaded Funtions to read the matrix from file
int getRatingMatrix(const char* filename, std:: vector<std::vector<double> >& ratingMatrix);
int getRatingMatrix(const char* filename, std::map<long, std::vector<double> >& ratingMatrix);

// Fisher Yates Shuffling Algorithm using templates
template<class fwditer>fwditer random_unique(fwditer begin, fwditer end, size_t num_random)
{
	// Find the Number of Elements between begin and end
	size_t left = std::distance(begin, end);
	
	if(left < num_random)
	{
		std::cout<<"Cannot generate enough Random Variables.\n";
		return begin;
	}
	
	// Seed for different random number everytime
	srand(time(0));

	// While we do not have num_random numbers
	while (num_random--)
	{
		// Set forwarditerator to the current begin, which is the end of the unique
		fwditer r = begin;
		
		// Advance the iterator randomly between begin() and end()
		std::advance(r, rand()%left);

		// Swap with begin and update the begin and left values
		std::swap(*begin, *r);
		++begin;
		 --left;
	}
	
	return begin;
}

#endif

