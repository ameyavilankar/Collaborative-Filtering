#include "main.h"

using namespace std;
using namespace PipeFish;

int main()
{
    cout<<"Reading in the Rating Matrix...\n";
    // Create the userIndex and the movieIndex
    std::map<long, int> userIndex;
    std::map<long, int> movieIndex;
    vector<vector<double> > ratingMatrix;

    // Get the rating matrix from the file
    int errorVal = getRatingMatrix("ratings.txt", userIndex, movieIndex, ratingMatrix);
	if(errorVal != 0)
        return errorVal;



    cout<<"Creating the global MovieMap and calculate the average Movie Ratings...\n";
    // read the movies into the global MovieMap
    map<long, Movie> movieMap;
    errorVal = getMovieMap("Movies.dat", movieMap);
    if(errorVal != 0)
        return errorVal;

    // Calculate the global average rating of the movies.
    double sum = 0.0;
    long count = 0;
    for(map<long, Movie>::iterator movie_iter = movieMap.begin(); movie_iter != movieMap.end(); movie_iter++)
    {
        // Calculate the average only over the non-zero ratings
        sum = 0.0;
        count = 0;
        for(int user = 0; user < ratingMatrix.size(); user++)
            if(ratingMatrix[user][movieIndex[movie_iter->first]] != 0)
            {
                sum += ratingMatrix[user][movieIndex[movie_iter->first]];
                count++;
            }

        // set the average for the movie
        movie_iter->second.setAverageRating(sum/count);
    }



    cout<<"Geting the userToClusterMap and the clusterToUserMap...\n";
    map<long, int> userToClusterMap;
    map<int, vector<long> > clusterToUserMap;
    errorVal = getUserToClusterMap("data.txt", userToClusterMap, clusterToUserMap);
    if(errorVal != 0)
        return errorVal;
    cout<<"No. of users: "<<userToClusterMap.size()<<"\n";
    cout<<"No. of Clusters:"<<clusterToUserMap.size()<<"\n\n";




    cout<<"Creating the global UserMap...\n";
    // Create the global userIndex
    std::map<long, User> userList;
    for(std::map<long, int>::const_iterator it = userIndex.begin(); it != userIndex.end(); it++)
    {
        // add the user to the global user list using its id as index
        userList[it->first] = User(it->first, userToClusterMap[it->first]);

        // set the rating vector for the user
        userList[it->first].setRatingVector(ratingMatrix[it->first], movieIndex);
    }

    // Replace by unordered_map or unordered_set
    map<int, Cluster> clusterMap;
    map<int, long> clusterCenterUser;  // TODO: read the map from file
    vector<long> userVector;
    map<long, User> clusterUserMap;

    for(map<int, vector<long> >::const_iterator cluster_it = clusterToUserMap.begin(); cluster_it != clusterToUserMap.end(); cluster_it++)
    {
        // Get the vector for the current cluster
        userVector = clusterToUserMap[cluster_it->first];

        // Get the corresponding user in a map
        for(int i = 0; i < userVector.size(); i++)
            clusterUserMap[userVector[i]] = userList[userVector[i]];

        // Create and add the cluster to the clusterMap
        clusterMap[cluster_it->first] = Cluster(cluster_it->first, ratingMatrix[userIndex[clusterCenterUser[cluster_it->first]]], clusterUserMap);

        clusterMap[cluster_it->first].buildMovieUniverse(movieMap);
        clusterMap[cluster_it->first].recommendMoviesToUser(movieMap);
    }


    cout<<"Clusters and their corresponding Users SORTED according to the userid: "<<endl;
    for(std::map<int, Cluster >::const_iterator it = clusterMap.begin(); it != clusterMap.end(); it++)
    {
        cout<<"Cluster No.: "<<it->first<<endl;

        // Output all the users in that cluster
        std::map<long, User> clusterUsers = it->second.getUsersFromCluster();

        for(std::map<long, User>::const_iterator user_it = clusterUsers.begin(); user_it != clusterUsers.end(); user_it++)
            cout<<user_it->second.getUserId()<<" ";
        
        cout<<endl;
    }

    //Create the universe of all movies
    map<long, Movie> movieUniverse;

    // Create a map of all users
    map<long, User> allUsers;

    // Create a map of Clusters
    map<int, Cluster> allClusters;

    // for(map<long, map<long, double> >::const_iterator it = ratingMatrix.begin(); it != ratingMatrix.end(); it++)
    // {
        
    // }

	return 0;
}