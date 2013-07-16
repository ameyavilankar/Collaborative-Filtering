#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include <sstream>
#include "visualise.h"

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

void calculateClusterCenters(map<int, vector<long> >& clusterToUserMap, map<long, vector<double> >& ratingMatrix, map<int, map<long, int> >& clusterUserDistances, map<int, vector<double> >& clusterCenters, map<int, long>& clusterCenterUser)
{
	// Keep track of the number of users
	int numberOfUsers = 0;

	for(map<int, vector<long> >::iterator it = clusterToUserMap.begin(); it != clusterToUserMap.end(); it++)
	{
		//cout<<"Cluster No: "<<it->first<<", Number of users: "<<it->second.size()<<endl;
		numberOfUsers += it->second.size();

		vector<double> currentCenter(ratingMatrix[it->second[0]].size());
		vector<double> temp;
		
		//cout<<"Size of current: "<<currentCenter.size()<<" ";
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

		double minDistance = std::numeric_limits<double>::max();
		long  minUser = 0;
		double currentDistance = 0.0;

		for(int i = 0; i < it->second.size(); i++)
		{
		    currentDistance  = exp(-cosineSimilarity(ratingMatrix[it->second[i]], currentCenter));
		    
		    if(currentDistance < minDistance)
		    {
			minDistance = currentDistance;
			minUser = it->second[i];
		    }
		}
	
		// Set it as the center of the current cluster
		clusterCenters[it->first] = ratingMatrix[minUser];
		clusterCenterUser[it->first] = minUser;

		for(int i = 0; i < it->second.size(); i++)
		{
		    clusterUserDistances[it->first][it->second[i]] =
		    (int)(exp(-cosineSimilarity(ratingMatrix[it->second[i]], currentCenter)/H) * 1000);
		}
	}
	
	cout<<"Number of Clusters:"<<clusterCenters.size()<<"\n";
	cout<<"Number of Users: "<<numberOfUsers<<"\n\n";

}

map<long, int> getCheckMap(const map<long, vector<double> >& ratingMatrix, map<int, vector<double> >& clusterCenters)
{
	map<long, int> checkMap;

	for(map<long, vector<double> >::const_iterator rating_iter = ratingMatrix.begin(); rating_iter != ratingMatrix.end(); rating_iter++)
	{
	    // Use only if want to see al the distaces
	    //This will hold the Canberra distances for the current user
	    vector<double> distance(clusterCenters.size());

	    //cout<<"Calculating Cluster Assignment for user: "<<rating_iter->first<<endl;

	    double minValue = std::numeric_limits<double>::max();
	    double currentDistance = 0.0;
	    int minIndex = 0;

	    for(map<int, vector<double> >::iterator cluster_it = clusterCenters.begin(); cluster_it != clusterCenters.end(); cluster_it++)
	    {
			//cout<<cluster_it->first<<" ";

			// Calculate the canberra distance
			currentDistance = exp(-cosineSimilarity(vector<double>(rating_iter->second.begin(), rating_iter->second.end()), vector<double>(cluster_it->second.begin(), cluster_it->second.end())));
				
			// TODO:: TODO:: check for multiple matches to minvalue
			if(currentDistance < minValue)
			{
			    minValue = currentDistance;
			    minIndex = cluster_it->first;
			}

	    }

	    // Set the cluster for the corresponding user
	    checkMap[rating_iter->first] = minIndex;		
	}

	return checkMap;
}

map<int, map<int, int> > calculateClusterToClusterDistances(map<int, vector<double> >& clusterCenters)
{
	map<int, map<int, int> > clusterToClusterDistances;
	int count = 0;
	for(map<int, vector<double> >::const_iterator it = clusterCenters.begin(); it != clusterCenters.end(); it++)
	{
		map<int, vector<double> >::const_iterator second_it = it;
		second_it++;
		while(second_it != clusterCenters.end())
		{
			count++;
			//cout<<"Calculating: "<<it->first<<" "<<second_it->first<<endl<<" "<<count<<endl;
			int distance = (int)(exp(-cosineSimilarity(it->second, second_it->second)/H) * 1000);
			clusterToClusterDistances[it->first][second_it->first] = distance;
			second_it++;
		}
	}
	
	cout<<"ClusterDistanceMatrixDimenstions: "<<clusterToClusterDistances.size()<<" , "<<clusterToClusterDistances.begin()->second.size()<<"\n\n";
	
	return clusterToClusterDistances;
}

void saveCheckMap(map<long, int>& userToClusterMap, map<long, int>& checkMap)
{
	int correct = 0;

	// Save the Checkcluster to file TODO: Rewrite as a function
	ofstream outfile;
	outfile.open("CheckCluster.txt");

	// Compare the distances and Check if right.
	outfile<<"User:\t Cluster1\t Cluster2\n";
	
	for(map<long, int>::const_iterator it = userToClusterMap.begin(); it != userToClusterMap.end(); it++)
	{
	    if(it->second == checkMap[it->first])
		correct++;
	    outfile<<it->first<<"\t"<<it->second<<"\t"<<checkMap[it->first]<<endl;
	}
	
	cout<<"Correct:"<<correct<<"/"<<userToClusterMap.size()<<"\n\n";
	outfile.close();
}

void saveJSONFiles(map<int, vector<long> >& clusterToUserMap, map<int, map<int, int> >& clusterToClusterDistances, map<int, map<long, int> >& clusterUserDistances)
{
	ofstream outfile1, outfile2;
	outfile1.open("nodes.json");
	outfile2.open("links.json");
		
	// save to file the first line of the file
	outfile1<<"{\n\"nodes\":[\n";
	outfile2<<"{\n\"links\":[\n";
	
	//print the first entry of the file
	//outfile2<<"{\"source\":"
	//outfile1<<"{\"name\":\""<<clusterToUserMap.begin()->second[0]<<"\",\"group\":"<<clusterToUserMap.begin()->first<<"}";
	
	int lineCount = 0;
	map<int, int> clusterLineMap;
	int linkCount = 0;

	for(map<int, vector<long> >::const_iterator it = clusterToUserMap.begin(); it != clusterToUserMap.end(); it++)
	{
		// Enter the cluster to the file
		outfile1<<"{\"name\":\"c"<<it->first<<"\",\"group\":"<<it->first<<"},\n";

		clusterLineMap[it->first] = lineCount++;
	
		for(int i = 0; i < it->second.size(); i++)
		{
			//outfile1<<"{\"name\":\""<<it->second[i]<<"\",\"group\":"<<it->first<<"},\n";
			//outfile2<<"{\"source\":"<<clusterLineMap[it->first]<<",\"target\":"<<lineCount<<",\"value\":"<<clusterUserDistances[it->first][it->second[i]]<<"},\n";
			//lineCount++;
			linkCount++;
		}
	}

	cout<<"LineCount: "<<lineCount<<endl;
	cout<<"LinkCount: "<<linkCount<<endl;
	
	// Add all the lines from one cluster to another
	for(map<int, map<int, int> >::const_iterator it = clusterToClusterDistances.begin(); it != clusterToClusterDistances.end(); it++)
	{
		for(map<int, int>::const_iterator second_it = it->second.begin(); second_it != it->second.end(); second_it++)
		{
			linkCount++;
			//cout<<"Adding Links from: Cluster "<<it->first<<" to Cluster "<<second_it->first<<endl;
			outfile2<<"{\"source\":"<<clusterLineMap[it->first]<<",\"target\":"<<clusterLineMap[second_it->first]<<",\"value\":"<<clusterToClusterDistances[it->first][second_it->first]<<"},\n";
		}
	}
	
	cout<<"LinkCount at the end: "<<linkCount<<"\n\n";
	
	outfile1<<"\n]\n}";
	outfile2<<"\n]\n}";
	
	// Close the two writing files
	outfile1.close();
	outfile2.close();
}

void saveClusterCenters(map<int, long>& clusterCenterUser)
{
	ofstream outfile;
	outfile.open("clusterCenters.txt");

	cout<<"Number of Clusters: "<<clusterCenterUser.size()<<"\n";
	for(map<int, long>::const_iterator it = clusterCenterUser.begin(); it != clusterCenterUser.end(); it++)
		outfile<<it->first<<" "<<it->second<<"\n";

	outfile.close();
}

int calculateDistance(map<long, vector<double> >& ratingMatrix, long one, long two)
{
	return (int)(exp(-cosineSimilarity(ratingMatrix[one], ratingMatrix[two])/H) * 1000);
}

void saveClusters(map<int, vector<long> >& clusterToUserMap, map<int, map<long, int> >& clusterUserDistances, map<long, vector<double> >& ratingMatrix, map<int, long>& clusterCenterUser)
{
	// define the constant strings
	const string cluster = "cluster_";
	const string nodes = ".nodes.json";
	const string links = ".links.json";

	for(map<int, vector<long> >::const_iterator it = clusterToUserMap.begin(); it != clusterToUserMap.end(); it++)
	{
		ofstream outfile1, outfile2;
		outfile1.open((cluster + to_string(it->first) +  nodes).c_str());
		outfile2.open((cluster + to_string(it->first) +  links).c_str());
			
		// save to file the first line of the file
		outfile1<<"{\n\"nodes\":[\n";
		outfile2<<"{\n\"links\":[\n";
		
		//print the first entry of the file
		//outfile2<<"{\"source\":"
		//outfile1<<"{\"name\":\""<<clusterToUserMap.begin()->second[0]<<"\",\"group\":"<<clusterToUserMap.begin()->first<<"}";
		
		int lineCount = 0;
		map<long, int> userLineMap;
		int linkCount = 0;
		int group = 0;
		string name;

		for(int i = 0; i < it->second.size(); i++)
		{
			// Change color of the cluster center
			if(clusterCenterUser[it->first] == it->second[i])
			{
				cout<<it->second[i]<<"\n";
				group = it->first + 1;
				name = "c" + to_string(it->second[i]);
			}
			else
			{
			    name = to_string(it->second[i]);
			    group = it->first;
			}

			if(i == it->second.size() - 1)
				outfile1<<"{\"name\":\""<<name<<"\",\"group\":"<<group<<"}\n";
			else
				outfile1<<"{\"name\":\""<<name<<"\",\"group\":"<<group<<"},\n";

			// save the lineCount for the links.json file
			userLineMap[it->second[i]] = lineCount++;
		}	

		for(int i = 0; i < it->second.size() - 1; i++)
			for(int j = i + 1; j < it->second.size(); j++)
			{
				if(i == it->second.size() - 2)
					outfile2<<"{\"source\":"<<userLineMap[it->second[i]]<<",\"target\":"<<userLineMap[it->second[j]]<<",\"value\":"<<calculateDistance(ratingMatrix, it->second[i], it->second[j])<<"}\n";
				else
					outfile2<<"{\"source\":"<<userLineMap[it->second[i]]<<",\"target\":"<<userLineMap[it->second[j]]<<",\"value\":"<<calculateDistance(ratingMatrix, it->second[i], it->second[j])<<"},\n";

				linkCount++;
			}	
		
		cout<<"LineCount: "<<lineCount<<"\n";
		cout<<"LinkCount: "<<linkCount<<"\n\n";
		
		outfile1<<"]\n}";
		outfile2<<"]\n}";
		
		// Close the two writing files
		outfile1.close();
		outfile2.close();	
	}	
	
}

void saveNodes(map<long, int>& userToClusterMap, map<long, int>& checkMap, map<int, long>& clusterCenterUser)
{
	int correct = 0;

	// Save the Checkcluster to file TODO: Rewrite as a function
	ofstream outfile;
	outfile.open("Nodes.txt");

	for(map<long, int>::const_iterator it = userToClusterMap.begin(); it != userToClusterMap.end(); it++)
	{
	    outfile<<it->first<<", "<<it->second<<", ";

	    if(it->first == clusterCenterUser[it->second])
	    	outfile<<"true\n";
	    else
	    	outfile<<"false\n";
	}
	
	outfile.close();
}


void saveLinks(map<int, vector<long> >& clusterToUserMap, map<int, map<int, int> >& clusterToClusterDistances, map<int, map<long, int> >& clusterUserDistances, map<int, long>& clusterCenterUser, map<long, vector<double> >& ratingMatrix)
{
	// define the constant strings
	const string cluster = "cluster_";
	const string links = "_links.txt";

	{
		ofstream outfile;
		outfile.open("clusterLinks.txt");
			
		// Add all the lines from one cluster to another
		for(map<int, map<int, int> >::const_iterator it = clusterToClusterDistances.begin(); it != clusterToClusterDistances.end(); it++)
		{
			for(map<int, int>::const_iterator second_it = it->second.begin(); second_it != it->second.end(); second_it++)
			{
				outfile << clusterCenterUser[it->first] << ", " << clusterCenterUser[second_it->first] << ", " << clusterToClusterDistances[it->first][second_it->first] << "\n";
			}
		}
		
		outfile.close();
	}
	
	// Add links between all the users in one cluster
	for(map<int, vector<long> >::const_iterator it = clusterToUserMap.begin(); it != clusterToUserMap.end(); it++)
	{
		ofstream outfile;
		outfile.open((cluster + to_string(it->first) +  links).c_str());
		
		cout<<"For Cluster: "<<it->first<<"\n";
		for(int i = 0; i < it->second.size() - 1; i++)
			for(int j = i + 1; j < it->second.size(); j++)
			{
				outfile << it->second[i] << ", " << it->second[j] << ", " << calculateDistance(ratingMatrix, it->second[i], it->second[j]) << "\n";
			}

		outfile.close();
	}
}

void clusterQuality(map<int, map<long, int> > clusterUserDistances)
{
	map<int, double> avgClusterDistances;
	
	// for each cluster, calculate the average distance of all the users in the cluster t	
	for(map<int, map<long, int> >::const_iterator cluster_it = clusterUserDistances.begin(); cluster_it != clusterUserDistances.end(); cluster_it++)
	{
		for(map<long, int>::const_iterator user_it = cluster_it->second.begin(); user_it != cluster_it->second.end(); user_it++)
			avgClusterDistances[cluster_it->first] += user_it->second;

		avgClusterDistances[cluster_it->first] /= cluster_it->second.size();	
	}

	double overallavg = 0.0;
	for(map<int, double>::const_iterator it = avgClusterDistances.begin(); it != avgClusterDistances.end(); it++)
	{
		cout << "Cluster: " << it->first << ", " << "Avg distance: " << it->second << "\n";
		overallavg += it->second;
	}
		
	overallavg /= avgClusterDistances.size();
	cout << "Overall Average: " << overallavg << "\n";
}

int visualise()
{
	// STEP 1: Read the clusters from the data.txt file and 
	// Create a map from every user to its cluster number 
	cout<<"Geting the userToClusterMap and the clusterToUserMap...\n";
	map<long, int> userToClusterMap;
	map<int, vector<long> > clusterToUserMap;
	int errorVal = getUserToClusterMap("data.txt_1_of_1", userToClusterMap, clusterToUserMap);
	if(errorVal != 0)
		return errorVal;
	cout<<"No. of users: "<<userToClusterMap.size()<<"\n";
	cout<<"No. of Clusters:"<<clusterToUserMap.size()<<"\n\n";
	
	
	/*
	// STEP 2: Read the cluster centers from the "cluster.txt" file
	vector<vector<double> > clusterCentersRDim;	
	errorVal = getClusterCenters("cluster.txt", clusterCentersRDim);
	if(errorVal != 0)
		return errorVal;
	cout<<"Cluster Dimensions: "<<clusterCentersRDim.size()<<", "<<clusterCentersRDim[0].size()<<endl;
	*/

	
	// STEP 3: Read the ratingMatrix to get the user vectors
	cout<<"Geting the ratingMatrix...\n";
	map<long, vector<double> > ratingMatrix;
	errorVal =  getRatingMatrix("ratings_with_id.txt", ratingMatrix);
	if(errorVal != 0)
		return errorVal;
	cout<<"RatingMatrix Dimensions: "<<ratingMatrix.size()<<", "<<ratingMatrix[1].size()<<"\n\n";


	// Come up with the cluster centers, distances of users to their cluster centers, and the user who is at the center of each cluster
	cout<<"Calculating the cluster centers...\n";
	map<int, map<long, int> > clusterUserDistances;
	map<int, vector<double> > clusterCenters;
	map<int, long> clusterCenterUser;
	calculateClusterCenters(clusterToUserMap, ratingMatrix, clusterUserDistances, clusterCenters, clusterCenterUser);
	
	// STEP 4: Calculate the CANBERRA distance of every user to the all the clusters and
	// find the cluster no. corresponding to the minimum distance
	cout<<"Calculating the checkMap...\n\n";
	map<long, int> checkMap = getCheckMap(ratingMatrix, clusterCenters);
	

	// Develop the distance matrix from one cluster to another
	cout<<"Calculating the cluster to cluster center distances...\n";
	map<int, map<int, int> > clusterToClusterDistances = calculateClusterToClusterDistances(clusterCenters);

	// Save the checkMap, JSON files and cluster center users to the file
	cout<<"Saving the results...\n";
	cout<<"Saving the map...\n";
	saveCheckMap(userToClusterMap, checkMap);
	cout<<"Saving the cluster centers to the file...\n";
	saveClusterCenters(clusterCenterUser);

	cout << "Printing the cluster quality: \n";
	clusterQuality(clusterUserDistances);

	return 0;

}
