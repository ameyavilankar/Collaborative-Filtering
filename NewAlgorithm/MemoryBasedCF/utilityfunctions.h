#ifndef UTILITY_FUNCTIONS
#define UTILITY_FUNCTIONS

//utilityfunctions.h -verison 1.0
// Author Ameya Vilankar

#include <string>
#include <algorithm>
#include <vector>
#include <iostream>
#include <wctype.h>

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
std::vector<double> split(const std::string& s)
{
	std::vector<std::string> returnString;

	typedef std::string::size_type string_size;
	string_size i = 0;

	// invariant: we have processed characters `['original value of `i', `i)'
	while (i != s.size()) 
	{
		// ignore leading blanks
		// invariant: characters in range `['original `i', current `i)' are all spaces
		while (i != s.size() && isSpace(s[i]))
			++i;

		// find end of next word
		string_size j = i;
		// invariant: none of the characters in range `['original `j', current `j)' is a space
		while (j != s.size() && !isSpace(s[j]))
			++j;

		// if we found some nonwhitespace characters
		if (i != j) 
		{
			// copy from `s' starting at `i' and taking `j' `\-' `i' chars
			returnString.push_back(s.substr(i, j - i));
			i = j;
		}
	}

	std::vector<double> returnDouble;
	std::transform(returnString.begin(), returnString.end(), std::back_inserter(returnDouble), getDouble);
	
	return returnDouble;
}

template<class fwditer>fwditer random_unique(fwditer begin, fwditer end, size_t num_random)
{
	// Find the Number of Elements between begin and end
	size_t left = std::distance(begin, end);
	
	if(left < num_random || num_random < 0)
	{
		std::cout<<"Cannot generate enough Random Variables."<<std::endl;
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
