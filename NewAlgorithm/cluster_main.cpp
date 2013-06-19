#include "main.h"
#include "readMatrix.cpp"
#include "canberra.cpp"
#include "math.h"
#include "preprocess.h"
#include "preprocess.cpp"

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

inline void getUserMapAndVector(const std::vector<std::vector<double> >& ratingMatrix, std::map<long, int>& userMap, std::vector<long>& userVector)
{
	for(int i = 0; i <ratingMatrix.size(); i++)
	{
		userMap[ratingMatrix[i][0]] = i;
		userVector[i] = ratingMatrix[i][0];
	}
}

inline int readClusterCenterUsers(map<long, int>& clusterUsers, string filename)
{
	ifstream myfile(filename.c_str());						 // Open the file for getting the input
    std::string currentLine;						 // To hold the entire currentline
    std::vector<double> splitDouble;				 // To hold the double values from the currentline

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
			
		// add user to the cluster
		clusterUsers[splitDouble[1]] = splitDouble[0];
    }

    cout<<"Number of users selected from old cluster centers: "<<clusterUsers.size()<<"\n";

    return 0;
}

// Selects the cluster centers users to be among the R users and selects the remaining users randomly
void getRandomAndClusterUsers(vector<long>& userVector, map<long, int> clusterUsers)
{
	int count = 0;
	long temp  = 0;

	for(int i = 0; i < userVector.size(); i++)
	{
		// search for the user present in the cluster centers and move it to the front of the list
		if(clusterUsers.find(userVector[i]) != clusterUsers.end())
		{
		    cout<<"Match: "<<userVector[i]<<"\n";

		    // swap the two
		    temp = userVector[count];
		    userVector[count] = userVector[i];
		    userVector[i] = temp;
		    count++;
		}
	}

	cout<<"Count: "<<count<<" , ClusterUsers: "<<clusterUsers.size()<<"\n";

	// Randomly select the remaining number of users
	vector<long >::iterator newBegin = random_unique(userVector.begin() + count, userVector.end(), NUM_FEATURES - count);

	userVector.resize(NUM_FEATURES);
}

int main()
{
	// Create a map from the user id to the ratingMatrix
	vector<vector<double> > ratingMatrix;
	cout<<"Getting the RatingMatrix...\n";
	// Get the ratings into the map from the file
	int errorVal =  getRatingMatrix("ratings_with_id.txt", ratingMatrix);
	cout<<"RatingMatrix Dimensions: "<<ratingMatrix.size()<<", "<<ratingMatrix[0].size()<<endl;
	
	for(int i = 0 ; i < ratingMatrix[101].size(); i++)
	    cout<<ratingMatrix[101][i]<<" ";
	cout<<endl;
	
	cout<<"Calculating the userMap and the userVector...\n";
	// Get the userMap and the userVector
	map<long, int> userMap;
	vector<long> userVector(ratingMatrix.size());
	getUserMapAndVector(ratingMatrix, userMap, userVector);
	
	

	cout<<"Geting the userToClusterMap and the clusterToUserMap...\n";
	map<long, int> userToClusterMap;
	map<int, vector<long> > clusterToUserMap;
	errorVal = getUserToClusterMap("data.txt", userToClusterMap, clusterToUserMap);
	if(errorVal != 0)
		return errorVal;
	cout<<"No. of users: "<<userToClusterMap.size()<<"\n";
	cout<<"No. of Clusters:"<<clusterToUserMap.size()<<"\n\n";
	

	
	cout<<"Calculating the ratingMatrix and vector for Cluster 1...\n";
	vector<vector<double> > clusterRatingMatrix;
	for(int i = 0; i < clusterToUserMap[1].size(); i++)
		clusterRatingMatrix.push_back(ratingMatrix[userMap[clusterToUserMap[1][i]]]);
	cout<<"clusterRatingMatrix size: "<<clusterRatingMatrix.size()<<"\n";

	for(int i = 0; i < clusterRatingMatrix.size(); i++)
	    cout<<clusterRatingMatrix[i][0]<<" "<<clusterToUserMap[1][i]<<"\n";
	cout<<endl;

	
	
	cout<<"Calculating and Processing the distance Matrix...\n";
	// Calculate the cosine distances to the R randomly selected users
	vector<vector<double> > cosineDistances;

	// For each user in the matrix calculate the cosine distance with the NUM_FEATURES randomly selected users
	for(int i = 0; i < clusterRatingMatrix.size(); i++)
	{

		// This will hold the cosine distances for the current user
		vector<double> distance(clusterRatingMatrix.size() + 1);

		// The first entry holds the userid which is taken from the ratingMatrix
		distance[0] = clusterRatingMatrix[i][0];
		
		// Calculate the cosine similarity with the randomly selected users		
		for(int j = 0; j < clusterRatingMatrix.size(); j++)
		{	
			distance[j + 1]  = cosineSimilarity(vector<double>(clusterRatingMatrix[i].begin() + 1, clusterRatingMatrix[i].end()), vector<double>(clusterRatingMatrix[j].begin() + 1, clusterRatingMatrix[j].end()));
		}

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
		for(int j = 1; j < cosineDistances[i].size(); j++)
			outfile<<i + 1<<" "<<j<<" "<<cosineDistances[i][j]<<endl;
	}

	outfile.close();
	
	return 0;
}
