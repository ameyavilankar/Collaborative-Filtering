#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Function used to determine if the current character is a colon or not
inline bool isColon(char c)
{
	return c == ':';
}

double getDouble(std::string toConvert)
{
	return atof(toConvert.c_str());
}


// Funciton used to split the sentence into its individual parts
std::vector<double> split(const std::string& s)
{
	std::vector<std::string> returnString;

	typedef std::cstring::size_type string_size;
	string_size i = 0;

	// invariant: we have processed characters `['original value of `i', `i)'
	while (i != s.size()) 
	{
		// ignore leading blanks
		// invariant: characters in range `['original `i', current `i)' are all spaces
		while (i != s.size() && isColon(s[i]))
		++i;

		// find end of next word
		string_size j = i;
		// invariant: none of the characters in range `['original `j', current `j)' is a space
		while (j != s.size() && !isColon(s[j]))
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

	// for(int i = 0; i < returnDouble.size(); i++)
	//  cout<<returnDouble[i]<<" ";
	// cout<<endl;

	return returnDouble;
}