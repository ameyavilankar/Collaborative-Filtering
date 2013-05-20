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

	    Movie(long id):movieId(id) {}
	
	private:
	    long movieId;                                           // Used to uniquely identify a Movie
	    
	    //TODO: Other Movie Related Data
	};

	// Used for comparing two Movies
	bool operator<(const Movie& one, const Movie& two)
	{
	    return (one.getMovieId() < two.getMovieId());
	}
}

#endif