#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;


class Movie
{
private:
    long movieId;
    //TODO
};

class User
{
private:
    long userId;                                // Stores the id of the User.
    std::vector<Movie> ratedMovies;             // Stores the Movies rated by the User.
    std::vector<Movies> recommendedMovies;      // Stores the List of Movies not seen by the user but seen by other users in the cluster.
};


class Cluster
{
private:
    int clusterId;                              // Cluster Id to identify the cluster.
    std::vector<double> clusterCenter;          // Store the center of the cluster for Difference Calculations.
    std::vector<User> users;                    // Stores the list of users belonging to the cluster.
    std::map<Movie> movieUniverse;              // Universe of all the movies rated by users in the cluster
};


int main()
{ 
    // Replace by unordered_map or unordered_set
    std::map<int, std::vector<long> > clusterMap;

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
    long user;
    int clusterNo;

    // Till there is no output or u get an error, add the user to its correpsonding cluster.
    while(myfile>>user>>clusterNo)
      clusterMap[clusterNo].push_back(user);

    cout<<"Clusters and their correpsonding Users: "
    for(std::map<int, std::vector<long> >::const_iterator it = clusterMap.begin(); it != clusterMap.end(); it++)
    {
        cout<<"Cluster No.: "<<it->first()<<endl;

        // Output all the users in that cluster
        for(int i = 0; i < it->second().size(); i++)
            cout<<it->second[i]<<" ";

        cout<<endl;
    }

    // OPTIONAL STEP: For each Cluster sort the users according to their ids
    for(std::map<int, std::vector<long> >::iterator it = clusterMap.begin(); it != clusterMap.end(); it++)
        sort(it->second().begin(), it->second().end());

    cout<<"Clusters and their correpsonding Users SORTED according to the userid: "
    for(std::map<int, std::vector<long> >::const_iterator it = clusterMap.begin(); it != clusterMap.end(); it++)
    {
        cout<<"Cluster No.: "<<it->first()<<endl;

        // Output all the users in that cluster
        for(int i = 0; i < it->second().size(); i++)
            cout<<it->second[i]<<" ";
        
        cout<<endl;
    }


    return 0;
}