#include "main.h"

using namespace std;
using namespace PipeFish;

int main()
{
    // Create the userMap and the movieMap
    std::map<long, int>& userMap;
    std::map<long, int>& movieMap;

    // Get the rating matrix from the file
    std::vector<std::vector<double> > ratingMatrix = getRatingMatrix("ratings.txt", userMap, movieMap);
	
    
    for(std::map<long, int>::const_iterator it = userMap.begin(); it != userMap.end(); it++)
    {

    }



    // Replace by unordered_map or unordered_set
    std::map<int, Cluster> clusterMap;

    {
        // Open the file for getting the input
        ifstream myfile("data_with_id.txt");
        
        //Always test the file open.
        if(!myfile) 
        {
          cout<<"Error opening output file"<<endl;
          system("pause");
          return -1;
        }

        // Temporary Variables to read the Data
        long userNo;
        int clusterNo;

        // Till there is no output or u get an error, add the user to its correpsonding cluster.
        while(myfile>>userNo>>clusterNo)
        {
            clusterMap[clusterNo] = Cluster(clusterNo);
            clusterMap[clusterNo].addUser(User(userNo, clusterNo));
        }
        
        // Clear and close the ifstream
        myfile.clear();
        myfile.close();
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

    // Create a map of map to store the user ratings.
    map<long, map<long, double> > ratingMatrix;
    
    {
        // Open the ratings file to read in the user ratings for the movies.
        ifstream myfile("ratings_with_id.txt");

        if(!myfile) 
        {
          cout<<"Error opening output file"<<endl;
          system("pause");
          return -1;
        }

        // To hold the entire currentline
        std::string currentLine;

        // To hold the double values from the currentline
        std::vector<double> splitDouble;
        
        while (std::getline (myfile, currentLine))
        {
            //cout<<count<<endl;
            // Split the currentLine and only return the double parts
            splitDouble = split(currentLine);
            
            // Store the rating in the map
            ratingMatrix[splitDouble[0]][splitDouble[1]] = splitDouble[2];
        }

        cout<<"Number of Users: "<<ratingMatrix.size();

        for(int i = 0; i < ratingMatrix.size(); i++)
        {
            cout<<i<<" ";
            for(int j = 0; j < ratingMatrix[i].size(); j++)
                cout<<ratingMatrix[i][j]<<" ";
            cout<<endl;
        }

        // Clear and close the ifstream
        myfile.clear();
        myfile.close();
    }

    //Create the universe of all movies
    map<long, Movie> movieUniverse;

    // Create a map of all users
    map<long, User> allUsers;

    // Create a map of Clusters
    map<int, Cluster> allClusters;

    for(map<long, map<long, double> >::const_iterator it = ratingMatrix.begin(); it != ratingMatrix.end(); it++)
    {
        
    }

	return 0;
}