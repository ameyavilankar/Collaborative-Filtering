#include <string>
#include <iostream>
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include <sstream>

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
using std::stringstream;

// Used to get the map from the users to their assigned cluster
int getUserToClusterMap(const char* filename, map<long, int>& userToClusterMap, map<int, vector<long> >& clusterToUserMap)
{
    // Open the file for reading
    ifstream myfile(filename);

    //Always test the file open.
    if(!myfile) 
    {
      cout<<"Error opening output file"<<endl;
      return -1;
    }
	
    vector<double> splitDouble;
    string currentLine;

    // Read till the end of the file
    while (std::getline (myfile, currentLine)) 
    {
    	// Split the currentLine and only return the double parts
	splitDouble = split(currentLine);
	    
	// map from the user id to the corresponding cluster
	userToClusterMap[splitDouble[0]] = splitDouble[1];
	clusterToUserMap[splitDouble[1]].push_back(splitDouble[0]);
    }

    return 0;
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

int getRatingMatrix(const char* filename, vector<vector<double> >& ratingMatrix)
{
    ifstream myfile(filename);                       // Open the file for getting the input
    std::string currentLine;                         // To hold the entire currentline
    std::vector<double> splitDouble;                 // To hold the double values from the currentline
    int userCount = 0;                               // To keep track of the number users
    map<int, long> userMap;                          // To Map from the ratingMatrix Row to the actual Userid

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
        ratingMatrix.push_back(vector<double>(splitDouble.begin(), splitDouble.end()));
    }

    cout<<"Maximum Number of Users: "<<ratingMatrix.size()<<endl;
    cout<<"Maximum Number of Movies:"<<ratingMatrix[0].size()<<endl;

    cout<<"Rating Matrix Size: "<<ratingMatrix.size()<<", "<<ratingMatrix[0].size()<<endl;
    
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

int main()
{
    // Create a map from every user to its cluster number 
    cout<<"Geting the userToClusterMap and the clusterToUserMap...\n";
    map<long, int> userToClusterMap;
    map<int, vector<long> > clusterToUserMap;
    int errorVal = getUserToClusterMap("data.txt", userToClusterMap, clusterToUserMap);
    if(errorVal != 0)
        return errorVal;
    cout<<"No. of users: "<<userToClusterMap.size()<<"\n";
    cout<<"No. of Clusters:"<<clusterToUserMap.size()<<"\n\n";
    
    
   
    cout<<"Geting the ratingMatrix...\n";
    vector<vector<double> > ratingMatrix;
    errorVal =  getRatingMatrix("ratings_with_id.txt", ratingMatrix);
    if(errorVal != 0)
        return errorVal;
    cout<<"RatingMatrix Dimensions: "<<ratingMatrix.size()<<", "<<ratingMatrix[1].size()<<"\n\n";

    ofstream outfile;
    outfile.open("kmeans.txt");

    for(int i = 0; i < clusterToUserMap[1].size(); i++)
    {
        cout<<clusterToUserMap[1][i]<<" ";
        outfile<<clusterToUserMap[1][i]<<" ";
        for(int j = 0; j < ratingMatrix[i].size(); j++)
            outfile<<ratingMatrix[i][j]<<" ";
        outfile<<"\n";
    }
}