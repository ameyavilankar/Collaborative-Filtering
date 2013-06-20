#ifndef CLUSTER_H
#define CLUSTER_H

// cluster.h version 1.0
// Author: Ameya Vilankar
#include "user.h"

namespace PipeFish
{
	struct MovieInfo
	{
		Movie movie;
		double averageRating;
		int numRatings;

		MovieInfo(const Movie& m = Movie(), double avg = 0.0, int num = 0): movie(m), averageRating(avg), numRatings(num) {}
	};
	
	// Used to represent a single cluster.
	class Cluster
	{
	public: 	
	    // Get and Set Functions
	    inline int getClusterId() const { return clusterId; }
	    inline void setClusterId(int id) { clusterId = id; }
	    inline const std::vector<double> getClusterCenter() const { return clusterCenter; }
	    inline void setClusterCenter(const std::vector<double>& center) { clusterCenter = center; }
	    inline std::map<long, User> getUsersFromCluster() const { return users; }
		inline std::map<long, MovieInfo> geMovieUniverse() const { return movieUniverse; }
	    
	    // Cluster Constructors
	    Cluster(int id = 0) : clusterId(id) {}
	    Cluster(int id, std::vector<double> center, std::map<long, User> userMap): clusterId(id)
	    {
	    	clusterCenter = center;
	    	users = userMap;
	    } 
	    
	    // Add user to the cluster
	    inline void addUser(const User& user)
	    {
	    	//TODO: check if required to check for preseent in hastable or not
	        users[user.getUserId()] = user;
	    }

	    void buildMovieUniverse(std::map<long, Movie>& movieMap);
	    void recommendMoviesToUser(std::map<long, Movie>& movieMap);
	    
	private:
	    int clusterId;                                          // Cluster Id to identify the cluster.
	    std::vector<double> clusterCenter;                      // Store the center of the cluster for Difference Calculations.
	    std::map<long, User> users;                             // Stores the list of users belonging to the cluster.
	    std::map<long, MovieInfo> movieUniverse;                   // Universe of all the movies rated by users in the cluster and the average rating given to the movie
	};

	// Used to Compare two Clusters by their Ids
	bool operator<(const Cluster& one, const Cluster& two)
	{
	    return (one.getClusterId() < two.getClusterId());
	}

	bool operator==(const Cluster& one, const Cluster& two)
	{
		return (one.getClusterId() == two.getClusterId());
	}

	bool operator!= (const Cluster& one, const Cluster& two)
	{
		return (one.getClusterId() != two.getClusterId());
	}
}

#endif
