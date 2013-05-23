#include "canberra.h"
#include "readMatrix.h"
#include "readMatrix.cpp"
#include "canberra.cpp"
#include <limits>

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
	map<int, vector<long> > clusterToUserMap;

	int errorVal = getUserToClusterMap("data.txt", userToClusterMap, clusterToUserMap);
	if(errorVal != 0)
		return errorVal;
	cout<<"No. of users: "<<userToClusterMap.size()<<endl;
	cout<<"No. of Clusters:"<<clusterToUserMap.size()<<endl;

	/*
	// STEP 2: Read the cluster centers from the "cluster.txt" file
	vector<vector<double> > clusterCenters;	
	errorVal = getClusterCenters("cluster.txt", clusterCenters);
	if(errorVal != 0)
		return errorVal;
	cout<<"Cluster Dimensions: "<<clusterCenters.size()<<", "<<clusterCenters[0].size()<<endl;
	*/
		
	// STEP 3: Read the ratingMatrix to get the user vectors
	map<long, vector<double> > ratingMatrix;
	errorVal =  getRatingMatrix("ratings_with_id.txt", ratingMatrix);
	if(errorVal != 0)
		return errorVal;

	cout<<"RatingMatrix Dimensions: "<<ratingMatrix.size()<<", "<<ratingMatrix[1].size();
	

	// Come up with the cluster centers
	map<int, vector<double> > clusterCenters;
	int numberOfUsers = 0;

	for(map<int, vector<long> >::iterator it = clusterToUserMap.begin(); it != clusterToUserMap.end(); it++)
	{
		cout<<"Cluster No: "<<it->first<<", Number of users: "<<it->second.size()<<endl;
		numberOfUsers += it->second.size();

		vector<double> currentCenter(ratingMatrix[it->second[0]].size());
		vector<double> temp;
		
		cout<<"Size of current: "<<currentCenter.size()<<" ";

		for(int i = 0; i < it->second.size(); i++)
		{
			// Get the rating Matrix of the user in the cluster
			temp = ratingMatrix[it->second[i]];
			
			// Store the sum
			for(int i = 0; i < currentCenter.size(); i++)
				currentCenter[i] += temp[i];
		}
		
		// Get the mean of all the clusters 
		for(int i = 0; i < currentCenter.size(); i++)
			currentCenter[i] /= it->second.size();

		// Set it as the center of the current cluster
		clusterCenters[it->first] = currentCenter;
	}
	
	cout<<"Number of Clusters:"<<clusterCenters.size()<<endl;
	cout<<"Number of Users: "<<numberOfUsers<<endl;

	// STEP 4: Calculate the CANBERRA distance of every user to the all the clusters and
	// find the cluster no. corresponding to the minimum distance
	map<long, int> checkMap;


	for(map<long, vector<double> >::const_iterator rating_iter = ratingMatrix.begin(); rating_iter != ratingMatrix.end(); rating_iter++)
	{
		// Use only if want to see al the distaces
		//This will hold the Canberra distances for the current user
		//vector<double> distance(clusterCenters.size());

		cout<<"Calculating Cluster Assignment for user: "<<rating_iter->first<<endl;

		double minValue = std::numeric_limits<double>::max();
		double currentDistance = 0.0;
		int minIndex = 0;

		for(map<int, vector<double> >::iterator cluster_it = clusterCenters.begin(); cluster_it != clusterCenters.end(); cluster_it++)
		{
			cout<<cluster_it->first<<" ";

			// Calculate the canberra distance
			currentDistance = calculate_canberradist(vector<double>(rating_iter->second.begin(), rating_iter->second.end()), vector<double>(cluster_it->second.begin(), cluster_it->second.end()));

			if(currentDistance < minValue)
			{
				minValue = currentDistance;
				minIndex = cluster_it->first;
			}
		}

		cout<<endl;

		// Set the cluster for the corresponding user
		checkMap[rating_iter->first] = minIndex;		
	}

	
	/*
	vector<vector<double> > ratingArray;
	for(map<long, vector<double> >::const_iterator rating_iter = ratingMatrix.begin(); rating_iter != ratingMatrix.end(); rating_iter++)
		ratingArray.push_back(rating_iter->second);
	
	cout<<"Rating Array Dimensions:"<<ratingArray.size()<<", "<<ratingArray[0].size()<<endl;

	vector<vector<double> > clusterArray;
	for(map<int, vector<double> >::const_iterator cluster_iter = clusterCenters.begin(); cluster_iter != clusterCenters.end(); cluster_iter++)
		clusterArray.push_back(cluster_iter->second);


	for(int i = 0; i < ratingArray.size(); i++)
	{
		// Use only if want to see al the distaces
		//This will hold the Canberra distances for the current user
		//vector<double> distance(clusterCenters.size());

		cout<<"Calculating Cluster Assignment for user: "<<(i + 1)<<endl;

		double minValue = std::numeric_limits<double>::max();
		double currentDistance = 0.0;
		int minIndex = 0;

		for(int j = 0; j < clusterArray.size(); j++)
		{
			cout<<(j + 1)<<" ";

			// Calculate the canberra distance
			currentDistance = calculate_canberradist(ratingArray[i], clusterArray[j]);

			if(currentDistance < minValue)
			{
				minValue = currentDistance;
				minIndex = j + 1;
			}
		}

		cout<<endl;

		// Set the cluster for the corresponding user
		checkMap[i + 1] = minIndex;		
	}
	*/

	// Compare the distances and Check if right.
	cout<<"User:\t Cluster1\t Cluster2\n";
    for(map<long, int>::const_iterator it = userToClusterMap.begin(); it != userToClusterMap.end(); it++)
    {
    	cout<<it->first<<"\t"<<it->second<<"\t"<<checkMap[it->first]<<endl;
    }

    return 0;

}
