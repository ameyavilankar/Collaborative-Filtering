#include "user.h"

using namespace PipeFish;

// Add a new Movie and its rating 
inline void User::addMovie(const Movie& newMovie, double rating) 
{
    // Check if movie is already rated by the user
    if(movieRatings.find(newMovie.getMovieId()) != movieRatings.end())
    {
        std::cout<<"Movie Already rated by the User."<<std::endl;
    }    
    
    // Movie not rated..so add to the map
    movieRatings[newMovie.getMovieId()] = rating;

    // TODO:: Also have to update the ratingVector
}

// Update a rating of an existing movie
inline void User::updateRating(const Movie& newMovie, double rating) 
{
    // Check if Movie is rated by the user
    if(movieRatings.find(newMovie.getMovieId()) == movieRatings.end())
    {
        std::cout<<"Movie Not Rated by the User. Add Movie First."<<std::endl;
        return;
    }
    
    // Movie rated by user..so update rating
    movieRatings[newMovie.getMovieId()] = rating;
    
    // TODO:: Also have to update the ratingVector
}

// std::vector<double> User::getRatingMatrix() const 
// {
//     for(int i = 0; i < universalMovieList.size(); i++)
//         ratingVector.push_back(movieRatings[universalMovieList[i].getMovieId()]);

//     //std::cout<<"Size of the RatingVector: "<<ratingVector.size()<<std::endl;

//     return ratingVector;
// }
