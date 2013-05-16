#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Using Declarations
using std::cin;
using std::cout;
using std::endl;
using std::sort;
using std::string;
using std::vector;
using std::ifstream;

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
vector<double> split(const string& s)
{
	vector<string> returnString;

	typedef string::size_type string_size;
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
	// 	cout<<returnDouble[i]<<" ";
	// cout<<endl;

	return returnDouble;
}



int main()
{
	// Open the file for getting the input
    ifstream myfile("ratings.dat");
    
    //Always test the file open.
    if(!myfile) 
    {
      cout<<"Error opening output file"<<endl;
      system("pause");
      return -1;
    }

	// have to find dynamically 
	int maxUserValue = 6040;
	int maxMovieValue = 3952;

	// The rating Matrix that will hold all the rating Data with the zeros
    vector<vector<double> > ratingMatrix(maxUserValue);

    // Create a maxUserValue X maxMovieValue Matrix of Zeros
    for(int i = 0; i < maxUserValue; i++)
    {
    	vector<double> rating(maxMovieValue);
    	ratingMatrix[i] = rating;
    }
    
    // To hold the entire currentline
    std::string currentLine;
    std::vector<double> splitDouble;

    //cout<<"Before main loop"<<endl;
    // Keep on reading till their are no lines
    while (std::getline (myfile, currentLine)) 
    {
    	// Split the currentLine and only return the double parts
 		splitDouble = split(currentLine);
 		ratingMatrix[splitDouble[0]-1][splitDouble[1]-1] = splitDouble[2];
    }

    cout<<"Number of Users: "<<maxUserValue<<endl;
    cout<<"Number of Movies: "<<maxMovieValue<<endl;
    cout<<"ratingMatrix: "<<endl;

    for(int i = 0; i < ratingMatrix.size(); i++)
    {	
    	cout<<i<<" ";
    	for(int j = 0; j < ratingMatrix[i].size(); j++)
    		cout<<ratingMatrix[i][j]<<" ";
    	cout<<endl;
    }

	return 0;
}