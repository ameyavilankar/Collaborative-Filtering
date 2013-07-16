#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <limits>
#include <stdio.h>
#include <algorithm>
#include <math.h>
#include <assert.h>
#include <fstream>
#include "constants.h"
#include "to_string.h"

using std::sort;
using std::map;
using std::vector;
using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::getline;
using std::ifstream;
using std::ofstream;
using std::min_element;
using std::distance;

double pearsonCoefficient(const std::vector<double>&, const std::vector<double>&);
double euclidean(const std::vector<double>&, const std::vector<double>&);

double calcPearson(const std::vector<double>& one, const std::vector<double>& two)
{
	std::vector<double> oneCommon;
	std::vector<double> twoCommon;
	
	for(int i = 0; i < one.size(); i++)
		if(one[i] != 0 && two[i] != 0)
		{
			oneCommon.push_back(one[i]);
			twoCommon.push_back(two[i]);
		}
	
	//std::cout<<"oneCommon: "<<oneCommon.size()<<" , TwoCommon: "<<twoCommon.size()<<std::endl;
	
	return pearsonCoefficient(oneCommon, twoCommon);
}

double calcEuclidean(const std::vector<double>& one, const std::vector<double>& two)
{
	std::vector<double> oneCommon;
	std::vector<double> twoCommon;
	
	for(int i = 0; i < one.size(); i++)
		if(one[i] != 0 && two[i] != 0)
		{
			oneCommon.push_back(one[i]);
			twoCommon.push_back(two[i]);
		}
	
	//std::cout<<"oneCommon: "<<oneCommon.size()<<" , TwoCommon: "<<twoCommon.size()<<std::endl;
	
	return euclidean(oneCommon, twoCommon);
}

// used to calculate the euclidean distance between two vectors
double euclidean(const std::vector<double>& one, const std::vector<double>& two)
{
  assert(one.size() == two.size());

  double sum = 0.0;
  for(int i = 0; i < one.size(); i++)
    sum += (one[i] - two[i]) * (one[i] - two[i]);

  return sum;
}

// used to calculate the mean of the vector
double mean(const std::vector<double> one)
{
  assert(one.size() >= 0);

  double mean = 0.0;
  for(int i = 0 ; i < one.size(); i++)
    mean += one[i];

  return mean/one.size();
}

// calculates the pearson coefficient between two vectors
double pearsonCoefficient(const std::vector<double>& one, const std::vector<double>& two)
{
  assert(one.size() == two.size());

  // Calculate the mean of the two vectors
  double meanOne = mean(one);
  double meanTwo = mean(two);

  // Calculate the square of difference and the product of the differences
  double oneSqrDiff = 0.0;
  double twoSqrDiff = 0.0;
  double prodDiff = 0.0;
  double oneDiff = 0.0;
  double twoDiff = 0.0;

  for(int i = 0; i < one.size(); i++)
  {
    oneDiff = (one[i] - meanOne);
    twoDiff = (two[i] - meanTwo);

    prodDiff += oneDiff * twoDiff;
    oneSqrDiff += oneDiff * oneDiff;
    twoSqrDiff += twoDiff * twoDiff;
  }
  
  double denominator = sqrt(oneSqrDiff * twoSqrDiff);

	if(denominator == 0)
	  return 0;

  return prodDiff/denominator;
}


// calculates the cosine similarity between two vectors
double cosineSimilarity(const std::vector<double>& one, const std::vector<double>& two)
{
	assert(one.size() == two.size());
	
	double oneSqrSum = 0.0;
	double twoSqrSum = 0.0;
	double prodSum = 0.0;
	
	for(int i = 0; i < one.size(); i++)
	{
		prodSum += (one[i] * two[i]);
		oneSqrSum += one[i] * one[i];
		twoSqrSum += two[i] * two[i];
  	}
	
	double denominator = sqrt(oneSqrSum * twoSqrSum);
	
	if(denominator == 0)
		return 0;
	else
		return prodSum/denominator;
}

// Function used to determine if the current character is a colon or not
inline bool isSpace(char c)
{
	return iswspace(c);
}

// Convert the string to its double equivalent
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

	/*
	 for(int i = 0; i < returnDouble.size(); i++)
	 	cout<<returnDouble[i]<<" ";
	 cout<<endl;
	*/

	return returnDouble;
}

int getRatingMatrix(const char* filename, map< long, vector<double> >& ratingMatrix)
{
	ifstream myfile(filename);						 // Open the file for getting the input
    std::string currentLine;						 // To hold the entire currentline
    std::vector<double> splitDouble;				 // To hold the double values from the currentline
	int userCount = 0;								 // To keep track of the number users

    //Always test the file open.
    if(!myfile) 
    {
      cout<<"Error opening output file"<<endl;
      return -1;
    }
	
	// Read till the end of the file
	while (std::getline (myfile, currentLine)) 
    {
    	// Split the currentLine and only return the double parts
 		splitDouble = split(currentLine);
			
		// Add it to the ratingMatrix
		//ratingMatrix.push_back(vector<double>(splitDouble.begin() + 1, splitDouble.end()));
		ratingMatrix[splitDouble[0]] = vector<double>(splitDouble.begin() + 1, splitDouble.end());
    }

	//std::cout<<"Maximum Number of Users: "<<ratingMatrix.size()<<endl;
	//std::cout<<"Maximum Number of Movies:"<<ratingMatrix[1].size()<<endl;

	/*
	for(int i = 0; i < ratingMatrix.size(); i++)
    {	
    	cout<<i<<" ";
    	for(int j = 0; j < ratingMatrix[i].size(); j++)
    		cout<<ratingMatrix[i][j]<<" ";
    	cout<<endl;
    }
    */

	return 0; 
}

int getRatingMatrix(const char* filename, vector<vector<double> >& ratingMatrix)
{
    ifstream myfile(filename);						 // Open the file for getting the input
    std::string currentLine;						 // To hold the entire currentline
    std::vector<double> splitDouble;				 // To hold the double values from the currentline
    int userCount = 0;								 // To keep track of the number users
    map<int, long> userMap;							 // To Map from the ratingMatrix Row to the actual Userid

    //Always test the file open.
    if(!myfile) 
    {
      cout<<"Error opening output file"<<endl;
      return -1;
    }
	
    // Read till the end of the file
    while (std::getline (myfile, currentLine)) 
    {
    	// Split the currentLine and only return the double parts
	splitDouble = split(currentLine);
		
	// Create a map from user_id to index the rating matrix
	if(userMap.find(userCount) == userMap.end())
	{
	    userMap[userCount] = splitDouble[0];
	    userCount++;
	}
		
	// Add it to the ratingMatrix
	//ratingMatrix.push_back(vector<double>(splitDouble.begin() + 1, splitDouble.end()));
	ratingMatrix.push_back(vector<double>(splitDouble.begin(), splitDouble.end()));
    }

    

    return 0; 
}


template<class fwditer>fwditer random_unique(fwditer begin, fwditer end, size_t num_random)
{
	// Find the Number of Elements between begin and end
	size_t left = std::distance(begin, end);
	
	if(left < num_random || num_random < 0)
	{
		cout<<"Cannot generate enough Random Variables."<<endl;
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


int visualise();