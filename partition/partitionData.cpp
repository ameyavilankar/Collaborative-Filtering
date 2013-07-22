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
#include <assert.h>

using namespace std;

// Used to split the string based on spaces
// Function used to determine if the current character is a colon or not
inline bool isColon(char c)
{
	return c == ':';
}

// Convert the string to its double equivalent
inline double getDouble(std::string toConvert)
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

	/*
	 for(int i = 0; i < returnDouble.size(); i++)
	 	cout<<returnDouble[i]<<" ";
	 cout<<endl;
	*/

	return returnDouble;
}

int getRatingMatrix(const char* filename, map<long, map<long, double> >& ratingMap)
{
	ifstream myfile(filename);						 // Open the file for getting the input
    std::string currentLine;						 // To hold the entire currentline
    std::vector<double> splitDouble;				 // To hold the double values from the currentline
	int userCount = 0;								 // To keep track of the number users
	map<long, int> movieMap;

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
			
		ratingMap[splitDouble[0]][splitDouble[1]] = splitDouble[2];

		movieMap[splitDouble[1]]++;
    }

	std::cout << "Maximum Number of Users: " << ratingMap.size() << endl;
	std::cout << "Maximum Number of Movies: " << movieMap.size() << endl;

	return 0; 
}


double cosineSimilarity(map<long, double>& one, map<long, double>& two)
{
	map<long, double> oneCommon, twoCommon;

	// find the common elements
	for(map<long, double>::const_iterator one_it = one.begin(); one_it != one.end(); one_it++)
		if(two.find(one_it->first) != two.end())
		{
			// Found common movie..add to oneCommon and twoCommon
			cout << one_it->first << " ";
			oneCommon[one_it->first] = one_it->second;
			twoCommon[one_it->first] = two[one_it->first];
		}

	cout << "\n";

	assert(oneCommon.size() == twoCommon.size());

	double oneSqrSum = 0.0;
	double twoSqrSum = 0.0;
	double prodSum = 0.0;
	
	for(map<long, double>::const_iterator one_it = oneCommon.begin(); one_it != oneCommon.end(); one_it++)
	{
		prodSum += (one_it->second * twoCommon[one_it->first]);
		oneSqrSum += one_it->second * one_it->second;
		twoSqrSum += twoCommon[one_it->first] * twoCommon[one_it->first];
  	}
	
	double denominator = sqrt(oneSqrSum * twoSqrSum);
	
	if(denominator == 0)
		return 0;
	else
		return prodSum/denominator;
}


int main()
{
	cout<<"Geting the ratingMap...\n";
	map<long, map<long, double> > ratingMap;
	int errorVal =  getRatingMatrix("ratings.dat", ratingMap);
	if(errorVal != 0)
		return errorVal;
	
	map<long, map<long, double> > graph;
	
	cout << "Calculating the Graph...\n";
	for(map<long, map<long, double> >::const_iterator user_it = ratingMap.begin(); user_it != ratingMap.end(); user_it++)
	{
		//cout << "Calculating edges for User: " << user_it->first << "\n";
		map<long, map<long, double> >::const_iterator other_user_it = user_it;
		other_user_it++;

		if(user_it == ratingMap.end() || other_user_it == ratingMap.end())
			continue;
		

		while(other_user_it != ratingMap.end()) 
		{
			for(map<long, double>::const_iterator movie_iter = user_it->second.begin(); movie_iter != user_it->second.end(); movie_iter++)
				if(other_user_it->second.find(movie_iter->first) != other_user_it->second.end())
				{
					// Both users have a commonly rated movie
					double cosine = cosineSimilarity(ratingMap[user_it->first], ratingMap[other_user_it->first]);
					
					if(user_it->first < other_user_it->first)
					    graph[user_it->first][other_user_it->first] = cosine;
				
					break;
				}

			other_user_it++;
		}
	}
	
	cout << "Number of vertices in graph:" << graph.size() << "\n";

	ofstream outfile;
	outfile.open("graph.txt");

	cout << "Saving the graph to file...\n";	

	for(map<long, map<long, double> >::const_iterator user_it = graph.begin(); user_it != graph.end(); user_it++)
		for(map<long, double>::const_iterator other_user_it = user_it->second.begin(); other_user_it != user_it->second.end(); other_user_it++)
			outfile << user_it->first << " " << other_user_it->first << " " << other_user_it->second << "\n";
	
	outfile.close();

}

/*
int main()
{

	map<long, double> one, two;

	one[1] = 1;
	one[2] = 2;
	one[3] = 3;

	two[1] = 3;
	two[2] = 4;
	two[3] = 5;
	//two[] = 6;

	cout << "Cosine Similairty: " << cosineSimilarity(one, two) << "\n";

	return 0;
}
*/