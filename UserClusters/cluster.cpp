#include "cluster.h"


// Calculates the cluster center by averagin the rating vectors of all the users in the cluster
void PipeFish::Cluster::calculateClusterCenter()
{
	// Reset the cluster center to all zeros
	for(size_t i = 0; i < clusterCenter.size(); i++)
		clusterCenter[i] = 0.0;

	// Sum the rating Vectors of all the users in the cluster
	std::vector<double> temp;
	for(std::map<long, User>::const_iterator it = users.begin(); it != users.end(); it++)
	{
		temp = it->second.getRatingVector();

		for(int j = 0; j < temp.size(); j++)
			clusterCenter[j] += temp[j];
	}

	// Take the mean of all the values
	for(int i = 0; i < clusterCenter.size(); i++)
		clusterCenter[i] /= users.size();
}

void PipeFish::Cluster::buildMovieUniverse()
{
	std::map<long, double> movieRatings;

	// Add all the rated movies to the universe and sum up their ratings and count the number of users rating each movie
	for(std::map<long, User>::const_iterator it = users.begin(); it != users.end(); it++)
	{
		movieRatings = it->second.getMovieRatings();

		for(std::map<long, double>::const_iterator it = movieRatings.begin(); it != movieRatings.end(); it++)
		{
			movieUniverse[it->first].averageRating += it->second;
			movieUniverse[it->first].numRatings++;	
		}
	}

	// Take the average of the ratings using the sum of ratings and the number of ratings
	for(std::map<long, MovieInfo>::iterator it = movieUniverse.begin(); it != movieUniverse.end(); it++)
		it->second.averageRating /=it->second.numRatings;
}