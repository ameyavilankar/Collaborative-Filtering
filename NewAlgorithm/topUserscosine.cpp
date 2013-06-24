#include "main.h"
#include "readMatrix.cpp"
#include "canberra.cpp"
#include "math.h"
#include "preprocess.h"
#include "preprocess.cpp"
#include <utility>

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
using std::make_pair;
using std::pair;

bool compare_zeros(const pair<long, int>& one, const pair<long, int>& two)
{
    return one.second > two.second;
}

// Returns the vector matrix, and two maps that map from user_id to row and movie_id to column of the rating matrix
int getRatingMatrix(const char* filename, map<long, int>& userMap, map<long, int>& movieMap, vector<vector<double> >& ratingMatrix)
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
	      return -1;
	    }
		
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
	    ratingMatrix = vector<vector<double> >(userMap.size(), vector<double>(movieMap.size()));

	    //Always test the file open.
	    if(!myfile) 
	    {
	    cout<<"Error opening output file"<<endl;
	    return -1;
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
 		ratingMatrix[userMap[(long)splitDouble[0]]][movieMap[(long)splitDouble[1]]] = splitDouble[2];
    }
	
	cout<<"Finished Loading Matrix:"<<endl;
	cout<<"Rating Matrix Size: "<<ratingMatrix.size()<<", "<<ratingMatrix[0].size()<<endl;
	
	
 	myfile.clear();
 	myfile.close();   

	return 0;
}




inline void getUserMapAndVector(const std::vector<std::vector<double> >& ratingMatrix,
std::map<long, int>& userMap, std::map<long, int> movieMap, std::vector<long>& userVector)
{
	vector<pair<long, int> > zeroVector;
	for(map<long, int>::const_iterator it = userMap.begin(); it != userMap.end(); it++)
	{
	    // calculate the non-zero entries
	    int nonZeroCount = 0;
	    for(int j = 0; j < ratingMatrix[it->second].size(); j++)
	    {
		if(ratingMatrix[it->second][j] != 0)
	    	    nonZeroCount++;
	    }
	    
	    zeroVector.push_back(make_pair<long, int>(it->first, nonZeroCount));
	}

	sort(zeroVector.begin(), zeroVector.end(), compare_zeros);

	for(int i = 0; i < NUM_FEATURES; i++)
	    userVector[i] = zeroVector[i].first;
}


int main()
{
	// Create a map from the user id to the ratingMatrix
	vector<vector<double> > ratingMatrix;
	map<long, int> userMap;
	map<long, int> movieMap;

	cout<<"Getting the RatingMatrix...\n";
	// Get the ratings into the map from the file
	int errorVal =  getRatingMatrix("ratings.dat", userMap, movieMap, ratingMatrix);
	cout<<"RatingMatrix Dimensions: "<<ratingMatrix.size()<<", "<<ratingMatrix[0].size()<<endl;
	
	cout<<"Calculating the userMap and the userVector...\n";
	vector<long> userVector(NUM_FEATURES);	
	getUserMapAndVector(ratingMatrix, userMap, movieMap, userVector);

	cout<<"UserVector size: "<<userVector.size()<<"\n";
	for(int i = 0; i < userVector.size(); i++)
	    cout<<userVector[i]<<" ";
	cout<<endl;

	cout<<"Calculating and Processing the distance Matrix...\n";
	// Calculate the cosine distances to the R randomly selected users
	vector<vector<double> > cosineDistances;

	// For each user in the matrix calculate the cosine distance with the NUM_FEATURES randomly selected users
	for(int i = 0; i < ratingMatrix.size(); i++)
	{
		// This will hold the cosine distances for the current user
		vector<double> distance(NUM_FEATURES);

		// Calculate the cosine similarity with the randomly selected users		
		for(int j = 0; j < NUM_FEATURES; j++)
		{	
			distance[j]  = cosineSimilarity(vector<double>(ratingMatrix[i].begin(), ratingMatrix[i].end()), vector<double>(ratingMatrix[userMap[userVector[j]]].begin(), ratingMatrix[userMap[userVector[j]]].end()));
		}
		
		cout<<"\n";
		// Add the distance to the cosine Distance Matrix
		cosineDistances.push_back(distance);
	}
	
	cout<<"Dimensions of the Cosine Distance Matrix: "<<cosineDistances.size()<<", "<<cosineDistances[0].size()<<endl;
	
	// Preprocess the matrix
	preprocessMatrix(cosineDistances);

	cout<<"Saving the Matrix to the file...\n";
	// Save it to the file
	ofstream outfile;
	outfile.open("SVDInput.txt");
	for(int i = 0; i < cosineDistances.size(); i++)
	{
		for(int j = 0; j < cosineDistances[i].size(); j++)
			outfile<<i + 1<<" "<<j + 1<<" "<<cosineDistances[i][j]<<endl;
	}

	outfile.close();
    
	return 0;
}
