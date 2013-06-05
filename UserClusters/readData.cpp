#include "readData.h"

// Using Declarations
using std::cin;
using std::cout;
using std::endl;
using std::sort;
using std::string;
using std::vector;
using std::map;
using std::ofstream;
using std::getline;
using std::ifstream;
using std::ofstream;
using std::min_element;
using std::distance;

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

// Returns the vector matrix, and two maps that map from user_id to row and movie_id to column of the rating matrix
vector<vector<double> > getRatingMatrix(const char* filename, map<long, int>& userMap, map<long, int>& movieMap)
{
	// To hold the entire currentline
	std::string currentLine;
	    
	// To hold the double values from the currentline
	std::vector<double> splitDouble;

	{
		// Open the file for getting the input
	    ifstream myfile(filename);
	    
	    //Always test the file open.
	    if(!myfile) 
	    {
	      cout<<"Error opening output file"<<endl;
	      return vector<vector<double> >();
	    }
		
		// To hold the entire currentline
	    std::string currentLine;
	    
	    // To hold the double values from the currentline
	    std::vector<double> splitDouble;

		int userCount = 0;
		int movieCount = 0;

		while (std::getline (myfile, currentLine)) 
	    {
			//cout<<count<<endl;
	    	// Split the currentLine and only return the double parts
	 		splitDouble = split(currentLine);
			
			// Create a map from user_id to index the rating matrix
			if(userMap.find(splitDouble[0]) == userMap.end())
			{
				userMap[splitDouble[0]] = userCount;
				userCount++;
			}

			// create a map from movie_id to index in the feature vector
			if(movieMap.find(splitDouble[1]) == movieMap.end())
			{
				movieMap[splitDouble[1]] = movieCount;
				movieCount++;
			}
	    }

		cout<<"Maximum Number of Users: "<<userMap.size()<<endl;
		cout<<"Maximum number of Movies: "<<movieMap.size()<<endl;
		
		/*cout<<"User Id"<<"\t"<<"Mapped id"<<endl;
		for(map<long, int>::const_iterator it = userMap.begin(); it != userMap.end(); it++)
			cout<<it->first<<"\t"<<it->second<<endl;
		
		//cout<<"Movie ID"<<"\t"<<"Mapped it"<<endl;
		int maxId = 0;
		for(map<long, int>::const_iterator it = movieMap.begin(); it != movieMap.end(); it++)
		{
			//cout<<it->first<<"\t"<<it->second<<endl;
			if(it->second > maxId)
				maxId = it->second;
		}

	    //cout<<"Maximum Mapped Movie Id"<<maxId<<endl;
		*/
	    
		// Clear the filestream so as to read again form the beginning 
		myfile.clear();
		myfile.close();
	}
	
	// To keep count of the number of lines read from file
    int count = 0;
   	
   	// Open the file for getting the input
    ifstream myfile(filename);
    
    // The rating Matrix that will hold all the rating Data with the zeros
	vector<vector<double> > ratingMatrix(userMap.size(), vector<double>(movieMap.size()));

    //Always test the file open.
    if(!myfile) 
    {
     	cout<<"Error opening output file"<<endl;
    	return vector<vector<double> >();
    }

   	cout<<"Before Loading Matrix:"<<endl;
    
    // Keep on reading till their are no lines
    while (std::getline (myfile, currentLine)) 
    {
		//cout<<count<<endl;
		count++;
    	// Split the currentLine and only return the double parts
 		splitDouble = split(currentLine);

		// Store the rating of the user for the corresponding movie.
 		ratingMatrix[userMap[(int)splitDouble[0]]][movieMap[(int)splitDouble[1]]] = splitDouble[2];
    }
	
	cout<<"Finished Loading Matrix:"<<endl;
	cout<<"Rating Matrix Size: "<<ratingMatrix.size()<<", "<<ratingMatrix[0].size()<<endl;
	
	for(int i = 0; i < ratingMatrix.size(); i++)
    {	
    	cout<<i<<" ";
    	for(int j = 0; j < ratingMatrix[i].size(); j++)
    		cout<<ratingMatrix[i][j]<<" ";
    	cout<<endl;
    }
 	
 	myfile.clear();
 	myfile.close();   

	return ratingMatrix;
}

// Used to calculate the distance between two vectors
double squareDistance(const std::vector<double>& a, const std::vector<double>& b)
{
	double total = 0;
	
	for (size_t i = 0;i < a.size(); ++i)
	{
		double d = a[i] - b[i];
		total += d * d;
	}
	return total;
}

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
	
	/*
	cout<<"User:\tCluster\n";
   	for(map<long, int>::const_iterator it = userToClusterMap.begin(); it != userToClusterMap.end(); it++)
    {
    	cout<<it->first<<"\t"<<it->second<<endl;
    }

	cout<<"Cluster:\tUser\n";
   	for(map<int, long>::const_iterator it = clusterToUserMap.begin(); it != clusterToUserMap.end(); it++)
    {
    	cout<<it->first<<"\t"<<it->second<<endl;
    }
	*/

    return 0;
}
