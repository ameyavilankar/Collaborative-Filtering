#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <map>

// Using Declarations
using std::cin;
using std::cout;
using std::endl;
using std::sort;
using std::string;
using std::vector;
using std::ifstream;
using std::map;
using std::ofstream;

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
	
	// Find the maximum movie and the user value
	int maxUserValue = 0, maxMovieValue = 0;
	
	// To hold the entire currentline
    std::string currentLine;
    
    // To hold the double values from the currentline
    std::vector<double> splitDouble;

	int count = 0;	
	map<long, int> movieMap;

	while (std::getline (myfile, currentLine)) 
    {
		//cout<<count<<endl;
    	// Split the currentLine and only return the double parts
 		splitDouble = split(currentLine);
		
		// Find the maximum id of the usr
		if(splitDouble[0] > maxUserValue)
			maxUserValue = splitDouble[0];
		
		// create a map from movie_id to index in the feature vector
		if(movieMap.find(splitDouble[1]) == movieMap.end())
		{
			movieMap[splitDouble[1]] = count;
			count++;
		}
    }

	maxMovieValue = movieMap.size();
	cout<<"Maximum Number of Users: "<<maxUserValue<<endl;
	cout<<"Maximum number of Movies: "<<maxMovieValue<<endl;
	
	// The rating Matrix that will hold all the rating Data with the zeros
    vector<vector<double> > ratingMatrix(maxUserValue, vector<double>(maxMovieValue));
    
	// Clear the filestream so as to read again form the beginning 
	myfile.clear();
	myfile.seekg(0);
	
    count = 0;
    
    // Keep on reading till their are no lines
    while (std::getline (myfile, currentLine)) 
    {
		//cout<<count<<endl;
		count++;
    	// Split the currentLine and only return the double parts
 		splitDouble = split(currentLine);

		// Store the rating of the user for the corresponding movie.
 		ratingMatrix[(int)splitDouble[0]-1][(int)movieMap[splitDouble[1]]-1] = splitDouble[2];
    }
	
	for(int i = 0; i < ratingMatrix.size(); i++)
    {	
    	cout<<i<<" ";
    	for(int j = 0; j < ratingMatrix[i].size(); j++)
    		cout<<ratingMatrix[i][j]<<" ";
    	cout<<endl;
    }
    
	return 0;
}
