#ifndef CLUSTER_H
#define CLUSTER_H

// cluster.h version 1.0
// Author: Ameya Vilankar
#include "user.h"

namespace PipeFish
{
	// Used to represent a single cluster.
	class Cluster
	{
	public:
	    // Get and Set Functions
	    inline int getClusterId() const { return clusterId; }
	    inline void setClusterId(int id) { clusterId = id; }

	    inline const std::vector<double> getClusterCenter() const { return clusterCenter; }
	    inline void setClusterCenter(const std::vector<double>& center) { clusterCenter = center; }

	    inline const std::map<long, User> getUsersFromCluster() const { return users; }

	    //inline const std::map<long, Movie> geMovieUniverse() const { return movieUniverse; }
	    
	    // Cluster Constructor
	    Cluster(int id = 0): clusterId(id) {}
	    
	    // Add user to the cluster
	    void addUser(const User& user)
	    {
	        // add the user to the map of users
	        users[user.getUserId()] = user;
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
}

#endif
