#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

// Used to represent a Movie which is identified by the movie_id
class Movie
{
public:
    // Get and Set functions
    inline long getMovieId() const { return movieId; }
    inline void setMovieId(long id) { movieId = id; }

    Movie(long id):movie_id(id) {}
private:
    long movieId;                                           // Used to uniquely identify a Movie
    
    //TODO: Other Movie Related Data
};

// Used for comparing two Movies
bool operator<(const Movie& one, const Movie& two)
{
    return (one.getMovieId() < two.getMovieId());
}


// Used to Represent a User and his movies
class User
{
public:
    // Get and Set Functions
    inline long getUserId() const { return userId; }
    inline void setUserId(long id) { userId = id; }
    
    //inline const std::map<const Movie, double> getUserMovies() const { return getUserMovies; }
    //inline std::map<Movie, double> getUserMovies() { return getUserMovies; }
    
    // Adds a new Movie that is rated by the user
    inline void addMovie(const Movie& newMovie, double rating = 3.0) 
    {
        // Check if movie is already rated by the user
        if(movieRatings.find(newMovie) != movieRatings.end())
        {
            cout<<"Movie Already rated by the User."<<endl;
        }    
        
        // Movie not rated..so add to the map
        movieRatings[newMovie] = rating;
    }


    inline void updateRating(const Movie& newMovie, double rating = 3.0) 
    {
        // Check if Movie is rated by the user
        if(movieRatings.find(newMovie) == movieRatings.end())
        {
            cout<<"Movie Not Rated by the User. Add Movie First."<<endl;
            return;
        }
        
        // Movie rated by user..so update rating
        movieRatings[newMovie] = rating;
    }

    // TODO: Add Movie by id
    User(long id): userId(id) {}

private:
    long userId;                                            // Stores the id of the User.
    std::map<Movie, double> movieRatings;                   // Stores the Movies rated by the User.
    std::vector<Movie> recommendedMovies;                   // Stores the List of Movies not seen by the user but seen by other users in the cluster. ORDERED??

    // TODO: Other user related data like name, etc.
};

// Used to compare two users by their id
bool operator<(const User& one, const User& two)
{
    return (one.getUserId() < two.getUserId());
}

// Used to represent a single cluster.
class Cluster
{
public:
    // Get and Set Functions
    inline int getClusterId() const { return clusterId; }
    inline void setClusterId(int id) { clusterId = id; }

    inline const std::vector<double> getClusterCenter() const { return clusterCenter; }
    inline void setClusterCenter(const std::vector<double>& center) { clusterCenter = center; }

    //inline const std::map<long, User> getUsersFromCluster() const { return users; }

    //inline const std::map<long, Movie> geMovieUniverse() const { return movieUniverse; }
    
    // Cluster Constructor
    Cluster(int id): clusterId(id) {}
    
    // Add user to the cluster
    void addUser(const User& user)
    {
        // add the user to the map of users
        users[user.getUserId()] = users;
    }
    
private:
    int clusterId;                                          // Cluster Id to identify the cluster.
    std::vector<double> clusterCenter;                      // Store the center of the cluster for Difference Calculations.
    std::map<long, User> users;                             // Stores the list of users belonging to the cluster.
    std::map<long, Movie> movieUniverse;                          // Universe of all the movies rated by users in the cluster
};

// Used to Compare two Clusters by their Ids
bool operator<(const Cluster& one, const Cluster& two)
{
    return (one.getClusterId() < two.getClusterId());
}

int main()
{ 
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
            cluserMap[clusterNo].addUser(User(userNo));
        }
         

        // Clear and close the ifstream
        myfile.clear();
        myfile.close();
    }
    
    // OPTIONAL STEP: For each Cluster sort the users according to their ids
    for(std::map<int, std::vector<long> >::iterator it = clusterMap.begin(); it != clusterMap.end(); it++)
        sort(it->second.begin(), it->second.end());

    cout<<"Clusters and their corresponding Users SORTED according to the userid: "<<endl;
    for(std::map<int, std::vector<long> >::const_iterator it = clusterMap.begin(); it != clusterMap.end(); it++)
    {
        cout<<"Cluster No.: "<<it->first<<endl;

        // Output all the users in that cluster
        for(int i = 0; i < it->second.size(); i++)
            cout<<it->second[i]<<" ";
        
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