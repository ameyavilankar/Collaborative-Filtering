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
#include <sstream>
#include <wctype.h>
#include <fstream>
#include <assert.h>

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

const double H = 1;

template<class T> std::string to_string(T n)
{
    std::ostringstream os;
    os << n;
    return os.str();
}

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

int calculateDistance(map<long, vector<double> >& ratingMatrix, long one, long two)
{
	return (int)(exp(-cosineSimilarity(ratingMatrix[one], ratingMatrix[two])/H) * 1000);
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
		userToClusterMap[splitDouble[0] - 1] = splitDouble[1];
		clusterToUserMap[splitDouble[1]].push_back(splitDouble[0] - 1);
    
    }
	
    /*
    cout<<"User:\tCluster\n";
    for(map<long, int>::const_iterator it = userToClusterMap.begin(); it != userToClusterMap.end(); it++)
    {
	cout<<it->first<<"\t"<<it->second<<endl;
    }
    */


    cout<<"Cluster:\tUser\n";
    for(map<int, vector<long> >::const_iterator it = clusterToUserMap.begin(); it != clusterToUserMap.end(); it++)
    {
	cout<<it->first<<"\n";
	
	for(int i = 0; i < it->second.size(); i++)
	    cout << it->second[i] << " " ;
	
	cout << "\n";
    }

    return 0;
}

// Used to get the graph from the graph file
int getGraph(const char* filename, map<long, map<long, double> >& graph)
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
	    
	    // Add the edge between the graphs
		graph[splitDouble[0]][splitDouble[1]] = splitDouble[2];
    }
	
	cout << "Number of vertices in the GRAPH: " << graph.size()	<< "\n";

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

void saveNodes(map<long, int>& userToClusterMap, map<int, long>& clusterCenterUser)
{
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

void saveGraph(int clusterNo, map<long, map<long, double> >& graph)
{
	cout << "Cluster No.: " << clusterNo << ", Number of vertices in graph:" << graph.size() << "\n";

	ofstream outfile;
	string filename = "graph_" + to_string(clusterNo) + ".txt";

	outfile.open(filename.c_str());

	cout << "Saving the graph to file...\n";	

	for(map<long, map<long, double> >::const_iterator user_it = graph.begin(); user_it != graph.end(); user_it++)
		for(map<long, double>::const_iterator other_user_it = user_it->second.begin(); other_user_it != user_it->second.end(); other_user_it++)
			outfile << user_it->first << " " << other_user_it->first << " " << other_user_it->second << "\n";
	
	outfile.close();

}

void saveIndividualGraphs(map<long, map<long, double> >& graph, map<int, map<long, double> >& clusterToUserMap, map<int, long>& clusterCenterUser, map<long, vector<double> >& ratingMatrix)
{
	// To hold the graph for each cluster
	map<int, map<long, map<long, double> > > clusterGraphs;

	for(map<int, map<long, double> >::const_iterator cluster_it = clusterToUserMap.begin(); cluster_it != clusterToUserMap.end(); cluster_it++)
	{
	    cout << "Calcualting for Cluster: " << cluster_it->first << "\n";
	    for(map<long, double>::const_iterator user_it = cluster_it->second.begin(); user_it != cluster_it->second.end(); user_it++)
		{
			// Get the edges from the current user
			map<long, double> edges = graph[user_it->first];
			
			cout << user_it->first << " ";
			for(std::map<long, double>::const_iterator other_user_it = edges.begin(); other_user_it != edges.end(); other_user_it++)
			{
				
				if(cluster_it->second.find(other_user_it->first) != cluster_it->second.end())
					clusterGraphs[cluster_it->first][user_it->first][other_user_it->first] = calculateDistance(ratingMatrix, user_it->first, other_user_it->first);
			}
		}

		cout << "\n";
	}

	// Save the individual graphs
	for(map<int, map<long, map<long, double> > >::iterator cluster_it = clusterGraphs.begin(); cluster_it != clusterGraphs.end(); cluster_it++)
		saveGraph(cluster_it->first, cluster_it->second);
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

int main()
{
	// STEP 1: Read the clusters from the data.txt file and 
	// Create a map from every user to its cluster number 
	cout<<"Geting the userToClusterMap and the clusterToUserMap...\n";
	map<long, int> userToClusterMap;
	map<int, vector<long> > clusterToUserMap;
	int errorVal = getUserToClusterMap("data.txt", userToClusterMap, clusterToUserMap);
	if(errorVal != 0)
		return errorVal;
	cout<<"No. of users: "<<userToClusterMap.size()<<"\n";
	cout<<"No. of Clusters:"<<clusterToUserMap.size()<<"\n\n";
	
	// STEP 2: Read the ratingMatrix to get the user vectors
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

	// Develop the distance matrix from one cluster to another
	cout<<"Calculating the cluster to cluster center distances...\n";
	map<int, map<int, int> > clusterToClusterDistances = calculateClusterToClusterDistances(clusterCenters);


	cout << "Getting the Graph...\n";
	map<long, map<long, double> > graph;
	errorVal = getGraph("graph.txt", graph);
	if(errorVal != 0)
		return errorVal;

	cout << "Creating the clusterToUserMap...\n";
	map<int, map<long, double> > clusterToUserMap2;
	for(map<int, vector<long> >::const_iterator cluster_it = clusterToUserMap.begin(); cluster_it != clusterToUserMap.end(); cluster_it++)
		for(int i = 0; i < cluster_it->second.size(); i++)
		clusterToUserMap2[cluster_it->first][cluster_it->second[i]] = 1.0;
	
	cout << "Save the Nodes to the file...\n";
	saveNodes(userToClusterMap, clusterCenterUser);

	// Save the checkMap, JSON files and cluster center users to the file
	cout<<"Saving the cluster graphs to the file...\n";
	saveIndividualGraphs(graph, clusterToUserMap2, clusterCenterUser, ratingMatrix);

	return 0;

}
