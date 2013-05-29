#ifndef MOVIE_H
#define MOVIE_H

// Movie.h - version 1.0
// Author: Ameya Vilankar

namespace PipeFish
{
	// Used to represent a Movie which is identified by the movie_id
	class Movie
	{
	
	public:
	    // Get and Set functions
	    inline long getMovieId() const { return movieId; }
	    inline void setMovieId(long id) { movieId = id; }
	    inline double getAverageRating() const {return averageRating; }
	    inline void setAverageRating(double avg) { averageRating = avg; }

	    Movie(long id = 0, double avg = 0.0):movieId(id), averageRating(avg) {}
	
	private:
	    long movieId;                                           // Used to uniquely identify a Movie
	    double averageRating;									// Stores the average rating given to the movie by all the users 
	    //TODO: Other Movie Related Data
	
	};

	// Used for comparing two Movies
	bool operator<(const Movie& one, const Movie& two)
	{
	    return (one.getMovieId() < two.getMovieId());
	}

	bool operator==(const Movie& one, const Movie& two)
	{
		return (one.getMovieId() == one.getMovieId());
	}

	bool operator!=(const Movie& one, const Movie& two)
	{
		return (one.getMovieId() != one.getMovieId());
	}

	// The Universal List that holds all the movies
	//std::vector<Movie> universalMovieList;
	//void initUniversalMovieList(char* filename);
}


#endif