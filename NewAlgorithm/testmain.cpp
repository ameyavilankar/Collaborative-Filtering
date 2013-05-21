#include "canberra.h"
#include "readMatrix.h"
#include "readMatrix.cpp"
#include "canberra.cpp"

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

// Used to get the map from the users to their assigned cluster
int getUserToClusterMap(const char* filename, map<long, int>& userToClusterMap)
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
    }

    cout<<"User:\tCluster\n";
    for(map<long, int>::const_iterator it = userToClusterMap.begin(); it != userToClusterMap.end(); it++)
    {
    	cout<<it->first<<"\t"<<it->second<<endl;
    }

    return 0;
}

// Used to read the cluster centers and store them in clusterCenters
int getClusterCenters(const char* filename, vector<vector<double> >& clusterCenters)
{
	// TODO
	ifstream myfile(filename);						 // Open the file for getting the input
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
		
		// TODO CHECKS for first element of the splitDouble
		clusterCenters.push_back(vector<double>(splitDouble.begin(), splitDouble.end()));
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
	// STEP 1: Read the clusters from the data.txt file and 
	// Create a map from every user to its cluster number 
	map<long, int> userToClusterMap;
	int errorVal = getUserToClusterMap("data.txt", userToClusterMap);
	if(errorVal != 0)
		return errorVal;
	cout<<"No. of users: "<<userToClusterMap.size()<<endl;
	
	// STEP 2: Read the cluster centers from the "cluster.txt" file
	vector<vector<double> > clusterCenters;	
	errorVal = getClusterCenters("cluster.txt", clusterCenters);
	if(errorVal != 0)
		return errorVal;
	cout<<"Cluster Dimensions: "<<clusterCenters.size()<<", "<<clusterCenters[0].size()<<endl;
	
	
	// STEP 3: Read the ratingMatrix to get the user vectors
	vector<vector<double> > ratingMatrix;
	errorVal =  getRatingMatrix("ratings_with_id.txt", ratingMatrix);
	if(errorVal != 0)
		return errorVal;
	cout<<"RatingMatrix Dimensions: "<<ratingMatrix.size()<<", "<<ratingMatrix[0].size();
	

	// STEP 4: Calculate the CANBERRA distance of every user to the all the clusters and
	// find the cluster no. corresponding to the minimum distance
	map<long, int> checkMap;

	for(int i = 0; i < ratingMatrix.size(); i++)
	{
		// This will hold the Canberra distances for the current user
		//vector<double> distance(clusterCenters.size());

		double minValue = std::numeric_limits<double>::max();
		double minDistCluster = 0;
		double currentDistance = 0.0;
		int minIndex = 0;

		for(int j = 0; j < clusterCenters.size(); j++)
		{
			// Call the canberra distance function
			currentDistance = calculate_canberradist(vector<double>(ratingMatrix[i].begin() + 1, ratingMatrix[i].end()), vector<double>(clusterCenters[j].begin(), clusterCenters[j].end()));

			if(currentDistance < minValue)
			{
				minValue = currentDistance;
				minIndex = j + 1; // CHECK
			}
		}

		// Set the cluster for the corresponding user
		checkMap[ratingMatrix[i][0]] = minIndex;		
	}

	// Compare the distances and Check if right.
	cout<<"User:\t Cluster1\t Cluster2\n";
    for(map<long, int>::const_iterator it = userToClusterMap.begin(); it != userToClusterMap.end(); it++)
    {
    	cout<<it->first<<"\t"<<it->second<<"\t"<<checkMap[it->first]<<endl;
    }

    return 0;

}