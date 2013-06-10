#ifndef COLLABORATIVE_FILTERING_H
#define COLLABORAIVE_FILTERING_H

// readPreferences.h -v1.0
// AUTHOR AMEYA VILANKAR

#include <map>
#include <ctime>
#include <string>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <utility>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <wctype.h>

#include "utilityfunctions.h"
#include "distances.h"

// Used to Load the dataset
int loadDataSet(char *moviefile, char *ratingsfile, std::map<long, std::map<std::string, double> >& userToMovie);

// Used to save the results to a file
int saveToFile(char* filename, const std::map<long, std::vector<std::pair<std::string, double> > >& recommendations);

// Convert the user-to-movie ratings to movie-to-user ratings
std::map<std::string, std::map<long, double> > transformPrefs(const std::map<long, std::map<std::string, double> >& userToMovie);

// Get the top N users similar to a user
std::vector<std::pair<long, double> > topMatchesUsers(std::map<long, std::map<std::string, double> >& userToMovie, long user, int n = 20);

// Get the top N movies similar to a movie
std::vector<std::pair<std::string, double> > topMatchesMovies(std::map<std::string, std::map<long, double> >& movieToUser, std::string movie, int n = 20);

// Get the positive correlated users corresponding to a user
std::vector<std::pair<long, double> > positiveCorrelationUsers(const std::map<long, std::map<std::string, double> >& userToMovie, long user, int n = 20);

// Get the recommended movies for user using user-based CF
std::vector<std::pair<std::string, double> > getRecommendations(const std::map<long, std::map<std::string, double> >& userToMovie, long user);

// Calculates top N similar for all users
void calculateSimilarUsers(std::map<long, std::map<std::string, double> >& userToMovie, int n = 20);

// Calculates top N similar for all movies
std::map<std::string, std::vector<std::pair<std::string, double> > > calculateSimilarMovies(std::map<std::string, std::map<long, double> >& movieToUser, int n = 20);


/*
Templated function to
template<class T1, class T2> std::vector<std::pair<T1, double> > topMatches(std::map<T1, std::map<T2, double> >& userToMovie, T1 user, int n)
{
	// To hold the users and the similarity scores
	typename std::vector<std::pair<T1, double> > scores;

	// Used for holding the common movies to cacluate the similarity scores
	std::vector<double> one, two;
	double score = 0.0;

	// Go through all other users except for the user
	for(typename std::map<T1, std::map<T2, double> >::const_iterator it = userToMovie.begin(); it != userToMovie.end(); it++)
		if(it->first != user)
		{
			for(typename std::map<T2, double>::const_iterator movie_iter = it->second.begin(); movie_iter != it->second.end(); movie_iter++)
				if(userToMovie[user].find(movie_iter->first) != userToMovie[user].end())
				{
					//Found Common Movie
					one.push_back(userToMovie[user][movie_iter->first]);
					two.push_back(movie_iter->second);
				}

			// Calculate the similarity score and add it to the vector
			score = pearsonCoefficient(one, two);
			scores.push_back(std::make_pair<T1, double>(it->first, score));
		}

	// sort them in the descending order of scores
	std::sort(scores.begin(), scores.end(), compare_scores_user);
	
	// keep only the top n elements
	scores.resize(n);

	return scores;
}
*/

#endif
