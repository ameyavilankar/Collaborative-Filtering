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
#include "distances.h"
#include "readMatrix.h"

// Using declarations
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

// Gets a mapping from the user to its cluster and cluster to its users
int getUserToClusterMap(const char* filename, map<long, int>& userToClusterMap, map<int, vector<long> >& clusterToUserMap);

// Used to read the cluster centers and store them in clusterCenters
int getClusterCenters(const char* filename, vector<vector<double> >& clusterCenters);
int getClusterCenters(const char* filename, map<int, vector<double> >& clusterCenters);


// Desciption at the end of the file
int visualise_cosine();

/*
1. Constructs a mapping from the user to the cluster.
2. Calculates the cluster mediods
3. Calculates the distance from each user in a cluster to its cluster center.
4. Verify the cluster assignments
5. Calculate the distances between all the cluster.
6. Save all the results in json and text files.
7. Calculate the Clustering Quality
*/