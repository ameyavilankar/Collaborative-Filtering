#ifndef USER_H
#define USER_H

// user.h version 1.0
// Author: Ameya Vilankar
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "movie.h"

namespace PipeFish
{
	// Used to Represent a User and his movies
	class User
	{
	
	public:
	    // Get and Set Functions
	    inline long getUserId() const { return userId; }
	    inline void setUserId(long id) { userId = id; }
	    
	    inline int getClusterId(int id) { return clusterId; }
	    inline void setClusterId(int id) { clusterId = id; }
	    
	    inline std::vector<double> getRatingVector() const { return ratingVector; }
	    inline void setRatingVector(std::vector<double> rat, std::map<long, int>& movieMap) 
	    { 
	    	ratingVector = rat;

	    	// Calculate the ratingsMap ???is it needed
        	calculateMovieRatings(movieMap);
	   	}
	    
	    inline const std::map<long, double> getMovieRatings() const { return movieRatings; }
	    void calculateMovieRatings(std::map<long, int>& movieMap);


	    inline std::vector<Movie> getRecommendedMovies() { return recommendedMovies; }
	    inline void setRecommendedMovies(std::vector<Movie> movies) { recommendedMovies = movie; }

	    // Adds a new Movie that is rated by the user
	    inline void addMovie(const Movie& newMovie, double rating = 3.0);
	    inline void updateRating(const Movie& newMovie, double rating = 3.0);

	    // TODO: Add Movie by id
	    User(long id = 0, int cId = 0): userId(id), clusterId(cId) {}

	private:
	    long userId;                                            // Stores the id of the User.
	    int clusterId;											// Stores the id of the cluster that the user belongs to
	    std::map<long, double> movieRatings;                    // Stores the Movies rated by the User.
	    std::vector<Movie> recommendedMovies;                   // Stores the List of Movies not seen by the user but seen by other users in the cluster. ORDERED?
	   	std::vector<double> ratingVector;						// Stores the rating Vector used for calculations

	    // TODO: Other user related data like name, etc.

	    //private functions
	};

	// Used to compare two users by their id
	bool operator<(const User& one, const User& two)
	{
	    return (one.getUserId() < two.getUserId());
	}

	// used for equality of two users
	bool operator==(const User& one, const User& two)
	{
		return (one.getUserId() == two.getUserId());
	}

	bool operator!=(const User& one, const User& two)
	{
		return (one.getUserId() != two.getUserId());
	}
}

#endif