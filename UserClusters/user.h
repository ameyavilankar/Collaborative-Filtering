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
	    
	    //inline const std::map<const Movie, double> getUserMovies() const { return getUserMovies; }
	    //inline std::map<Movie, double> getUserMovies() { return getUserMovies; }
	    
	    // Adds a new Movie that is rated by the user
	    inline void addMovie(const Movie& newMovie, double rating = 3.0);
	    
	    inline void updateRating(const Movie& newMovie, double rating = 3.0);
	    
	    // TODO: Add Movie by id
	    User(long id = 0): userId(id) {}

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

	// used for equality of two users
	bool operator==(const User& one, const User& two)
	{
		return (one.getUserId() == two.getUserId());
	}
}

#endif