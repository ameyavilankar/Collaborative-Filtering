#include "user.h"

// Add a new Movie and its rating 
inline void PipeFish::User::addMovie(const Movie& newMovie, double rating) 
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
inline void PipeFish::User::updateRating(const Movie& newMovie, double rating) 
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

void PipeFish::User::calculateMovieRatings(std::map<long, int>& movieMap)
{
    // Empty the current matrix
    movieRatings.erase(movieRatings.begin(), movieRatings.end());


    for(std::map<long, int>::const_iterator it = movieMap.begin(); it != movieMap.end(); it++)
    {
        // rating exists
        if(ratingVector[it->second] != 0)
        {
            movieRatings[it->first] = ratingVector[it->second];
        }
    }    
}






// std::vector<double> User::getRatingMatrix() const 
// {
//     for(int i = 0; i < universalMovieList.size(); i++)
//         ratingVector.push_back(movieRatings[universalMovieList[i].getMovieId()]);

//     //std::cout<<"Size of the RatingVector: "<<ratingVector.size()<<std::endl;

//     return ratingVector;
// }
