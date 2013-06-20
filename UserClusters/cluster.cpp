#include "cluster.h"

using std::map;
using std::vector;

void PipeFish::Cluster::buildMovieUniverse(std::map<long, Movie>& movieMap)
{
	std::map<long, double> movieRatings;

	// Add all the rated movies to the universe and sum up their ratings and count the number of users rating each movie
	for(std::map<long, User>::const_iterator it = users.begin(); it != users.end(); it++)
	{
		movieRatings = it->second.getMovieRatings();

		for(std::map<long, double>::const_iterator it = movieRatings.begin(); it != movieRatings.end(); it++)
		{
			movieUniverse[it->first].movie = movieMap[it->first];
			movieUniverse[it->first].averageRating += it->second;
			movieUniverse[it->first].numRatings++;	
		}
	}

	// Take the average of the ratings using the sum of ratings and the number of ratings
	for(std::map<long, MovieInfo>::iterator it = movieUniverse.begin(); it != movieUniverse.end(); it++)
		it->second.averageRating /=it->second.numRatings;
}

void PipeFish::Cluster::recommendMoviesToUser(std::map<long, Movie>& movieMap)
{

	for(std::map<long, User>::iterator user_it = users.begin(); user_it != users.end(); user_it++)
	{
		std::vector<Movie> movieVector;
	
		// get the movieRating map for the user
		std::map<long, double> ratings = user_it->second.getMovieRatings();

		for(std::map<long, MovieInfo>::iterator movie_it = movieUniverse.begin(); movie_it != movieUniverse.end(); movie_it++)
		{
			if(ratings.find(movie_it->first) == ratings.end())
				movieVector.push_back(movie_it->second.movie);
		}

		// OPTIONAL:: sort the vector according to the movie avd ratings or svd???
		// Set the vector for the movies
		user_it->second.setRecommendedMovies(movieVector);
	}

}