#include "collaborativefiltering.h"

// Using Declarations
using std::cin;
using std::cout;
using std::endl;
using std::sort;
using std::string;
using std::vector;
using std::pair;
using std::make_pair;
using std::map;
using std::ifstream;
using std::ofstream;
using std::getline;
using std::sort;

// To compare
bool compare_scores_user(const pair<long, double>& one, const pair<long, double>& two)
{
	if(one.second == two.second)
		return (one.first > two.first);
	else
		return (one.second > two.second);
}

bool compare_scores_movie(const pair<string, double>& one, const pair<string, double>& two)
{
	if(one.second == two.second)
		return (one.first > two.first);
	else
		return (one.second > two.second);
}

// Used to Load the dataset
int loadDataSet(const string& moviefile, const string& ratingsfile, map<long, map<string, double> >& userToMovie)
{
	string currentLine;						 // To hold the entire currentline
	map<long, string> movieMap;			 // Maps from the movie id to their  title

	{	
		vector<string> splitData;				 // To hold the double values from the currentline
		// Build a map from movie_ids to their titles
		ifstream infile(moviefile.c_str());						 // Open the file for getting the input

		//Always test the file open.
		if(!infile) 
		{
		  cout<<"Error opening output file"<<endl;
		  return -1;
		}
		
		// keep on reading till we get to the end of the file
		while(getline(infile, currentLine))
		{
			splitData = getMovieIdName(currentLine);
			//cout<<"Size: "<<splitData[0]<<splitData[1]<<endl;

			movieMap[(long)atof(splitData[0].c_str())] = splitData[1];
		}
		
		cout<<"Movie Map size: "<<movieMap.size()<<endl;
		
		infile.close();
	}
	
	{
		ifstream infile;
		infile.open(ratingsfile.c_str());
		vector<double> splitDouble;
		
		// Always tes the file
		if(!infile)
		{
			cout<<"Error Opening File"<<endl;
			return -1;
		}

		while(getline(infile, currentLine))
		{
			// Split the data into subparts
			splitDouble= split(currentLine);	

			// Store the rating for the user and the movie	
			userToMovie[splitDouble[0]][movieMap[splitDouble[1]]] = splitDouble[2];
		}
	}

	return 0;
}

void saveToFile(char* filename, const map<long, vector<pair<string, double> > >& recommendations)
{
	ofstream outfile;
	outfile.open(filename);

	for(map<long, vector<pair<string, double> > >::const_iterator it = recommendations.begin(); it != recommendations.end(); it++)
	{
		outfile<<it->first<<":\n";
		for(int i = 0; i < it->second.size(); i++)
			outfile<<it->second[i].first<<" "<<it->second[i].second<<"\n";
		cout<<"\n";
	}
	
	outfile.close();	
}

map<string, map<long, double> > transformPrefs(const map<long, map<string, double> >& userToMovie)
{
	// TO be returned
	map<string, map<long, double> > movieToUser;

	// Transform from user to movie rating to movie to user rating
	for(map<long, map<string, double> >::const_iterator user_iter = userToMovie.begin(); user_iter != userToMovie.end(); user_iter++)
		for(map<string, double>::const_iterator movie_iter = user_iter->second.begin(); movie_iter !=user_iter->second.end(); movie_iter++)
			movieToUser[movie_iter->first][user_iter->first] = movie_iter->second;

	return movieToUser;
}


vector<pair<long, double> > topMatchesUsers(map<long, map<string, double> >& userToMovie, long user, int n)
{
	// To hold the users and the similarity scores
	vector<pair<long, double> > scores;

	// Used for holding the common movies to cacluate the similarity scores
	vector<double> one, two;
	double score = 0.0;

	// Go through all other users except for the user
	for(map<long, map<string, double> >::const_iterator it = userToMovie.begin(); it != userToMovie.end(); it++)
		if(it->first != user)
		{
			for(map<string, double>::const_iterator movie_iter = it->second.begin(); movie_iter != it->second.end(); movie_iter++)
				if(userToMovie[user].find(movie_iter->first) != userToMovie[user].end())
				{
					//Found Common Movie
					one.push_back(userToMovie[user][movie_iter->first]);
					two.push_back(movie_iter->second);
				}

			// Calculate the similarity score and add it to the vector
			score = pearsonCoefficient(one, two);
			scores.push_back(make_pair<long, double>(it->first, score));
		}

	// sort them in the descending order of scores
	sort(scores.begin(), scores.end(), compare_scores_user);
	
	// keep only the top n elements
	scores.resize(n);

	return scores;
}


vector<pair<string, double> > topMatchesMovies(map<string, map<long, double> >& movieToUser, const string& movie, map<long, double>& averageRatings, int n)
{
	// To hold the movies and the similarity scores
	vector<pair<string, double> > scores;

	// Used for holding the common users to cacluate the similarity scores
	vector<double> one, two, user_avg;
	double score = 0.0;


	// TODO CHANGE: Find the movies according to algortihm
	// TODO TODO TODO
	// Go through all other movies except for the current movie
	for(map<string, map<long, double> >::const_iterator movie_iter = movieToUser.begin(); movie_iter != movieToUser.end(); movie_iter++)
		if(movie_iter->first != movie)
		{
			for(map<long, double>::const_iterator user_iter = movie_iter->second.begin(); user_iter != movie_iter->second.end(); user_iter++)
				if(movieToUser[movie].find(user_iter->first) != movieToUser[movie].end())
				{
					//Found Common user who rated both the movies
					one.push_back(movieToUser[movie][user_iter->first]);
					two.push_back(user_iter->second);
					user_avg.push_back(averageRatings[user_iter->first]);
				}

			// Calculate the similarity score and add it to the vector
			score = adjustedCosineSimilarity(one, two, user_avg);
			scores.push_back(make_pair<string, double>(movie_iter->first, score));
		}

	// sort them in the descending order of scores
	sort(scores.begin(), scores.end(), compare_scores_movie);
	
	// keep only the top n elements
	scores.resize(n);

	return scores;
}

vector<pair<long, double> > positiveCorrelationUsers(map<long, map<string, double> >& userToMovie, long user, int n)
{
	// To hold the users and the similarity scores
	vector<pair<long, double> > scores;
	
	// Used for holding the common movies to cacluate the similarity scores
	vector<double> one, two;
	double score = 0.0;

	// Go through all other users except for the user
	for(map<long, map<string, double> >::const_iterator it = userToMovie.begin(); it != userToMovie.end(); it++)
		if(it->first != user)
		{
			for(map<string, double>::const_iterator movie_iter = it->second.begin(); movie_iter != it->second.end(); movie_iter++)
				if(userToMovie[user].find(movie_iter->first) != userToMovie[user].end())
				{
					//Found Common Movie
					one.push_back(userToMovie[user][movie_iter->first]);
					two.push_back(movie_iter->second);
				}

			// Calculate the similarity score and add it to the vector if it is greater than zero
			score = pearsonCoefficient(one, two);
			if(score > 0.0)
				scores.push_back(make_pair<long, double>(it->first, score));
		}

	// sort them in the descending order of scores
	sort(scores.begin(), scores.end(), compare_scores_user);
	
	// keep only the top n elements
	scores.resize(n);

	return scores;
}

// Get the Top N recommendations
vector<pair<string, double> > getRecommendationsUBSingle(map<long, map<string, double> >& userToMovie, long user)
{
	//Create maps to hold the total of the weighted ratings and the total of the similarity scores
	map<string, double> totals;
	map<string, double> similaritySum;
	
	// Used for holding the common movies to cacluate the similarity scores
	vector<double> one, two;
	double score = 0.0;

	for(map<long, map<string, double> >::const_iterator it = userToMovie.begin(); it != userToMovie.end(); it++)
		if(it->first != user)
		{
			for(map<string, double>::const_iterator movie_iter = it->second.begin(); movie_iter != it->second.end(); movie_iter++)
				if(userToMovie[user].find(movie_iter->first) != userToMovie[user].end())
				{
					//Found Common Movie, add to vectors
					one.push_back(userToMovie[user][movie_iter->first]);
					two.push_back(movie_iter->second);
				}

			// Calculate the similarilty scores using commonly rated movies
			score = pearsonCoefficient(one, two);

			// Only consider user if positively correlated
			if(score > 0.0)
			{
				// For each movie rated by similar user
				for(map<string, double>::const_iterator movie_iter = it->second.begin(); movie_iter != it->second.end(); movie_iter++)
					if(userToMovie[user].find(movie_iter->first) == userToMovie[user].end() || userToMovie[user][movie_iter->first] == 0)
					{
						totals[movie_iter->first] += movie_iter->second * score;
						similaritySum[movie_iter->first] += score;
					}
			}
		}

	// To hold the top recommendations
	vector<pair<string, double> > recommendations;

	// for each movie in totals calculate their normalized scores
	for(map<string, double>::iterator it = totals.begin(); it != totals.end(); it++)
		recommendations.push_back(make_pair<string, double>(it->first, it->second/similaritySum[it->first]));
	
	// Sort them according to the predicted score
	sort(recommendations.begin(), recommendations.end(), compare_scores_movie);

	return recommendations;
}


vector<pair<string, double> > getRecommendationsIBSingle(map<long, map<string, double> >& userToMovie, map<string, vector<pair<string, double> > > topSimilarMovies, long user)
{
	// Get the user ratings
	map<string, double> userRatings = userToMovie[user];

	//Create maps to hold the total of the weighted ratings and the total of the similarity scores
	map<string, double> scores;
	map<string, double> totalSum;

	// There may be repeated movies counted for....TODO TODO TODO


	// Loop throuhg all the movies rated by the user
	for(map<string, double>::const_iterator movie_iter = userRatings.begin(); movie_iter != userRatings.end(); movie_iter++)
	{
		// Get the similar movies to the movie rated by the user
		vector<pair<string, double> > similarMovies = topSimilarMovies[movie_iter->first];

		// Loop over all of the similar movies
		for(vector<pair<string, double> >::const_iterator similar_iter = similarMovies.begin(); similar_iter != similarMovies.end(); similar_iter++)
		{
			// If the movie is not rated by the user
			if(userRatings.find(similar_iter->first) == userRatings.end())
			{
				// weight the score by the similarity score
				scores[similar_iter->first] += similar_iter->second * movie_iter->second;

				// Sum the similarity ratings for normalization
				totalSum[similar_iter->first] += similar_iter->second;

			}
		}
	}

	// To hold the top recommendations
	vector<pair<string, double> > recommendations;

	// for each movie in scores calculate their normalized scores
	for(map<string, double>::iterator it = scores.begin(); it != scores.end(); it++)
		recommendations.push_back(make_pair<string, double>(it->first, it->second/totalSum[it->first]));
	
	// Sort them according to the predicted score
	sort(recommendations.begin(), recommendations.end(), compare_scores_movie);

	return recommendations;

}


map<long, vector<pair<string, double> > > getRecommendationsUserBased(map<long, map<string, double> >& userToMovie)
{
	map<long, vector<pair<string, double> > > recommendedItemsUserBased;

	for(map<long, map<string, double> >::const_iterator user_iter = userToMovie.begin(); user_iter != userToMovie.end(); user_iter++)
	{
		//cout<<"User id: "<<user_iter->first<<endl;
		recommendedItemsUserBased[user_iter->first] = getRecommendationsUBSingle(userToMovie, user_iter->first);
	}

	return recommendedItemsUserBased;
}

map<long, vector<pair<string, double> > > getRecommendationsItemBased(map<long, map<string, double> >& userToMovie, map<string, vector<pair<string, double> > >& topSimilarMovies)
{
	map<long, vector<pair<string, double> > > recommendedItemsItemBased;

	for(map<long, map<string, double> >::const_iterator user_iter = userToMovie.begin(); user_iter != userToMovie.end(); user_iter++)
	{
		//cout<<"User id: "<<user_iter->first<<endl;
		recommendedItemsItemBased[user_iter->first] = getRecommendationsIBSingle(userToMovie, topSimilarMovies, user_iter->first);
	}

	return recommendedItemsItemBased;
}

map<long, vector<pair<long, double> > > calculateSimilarUsers(map<long, map<string, double> >& userToMovie, int n)
{
	map<long, vector<pair<long, double> > > similarUsers;

	int count = 0;
	for(map<long, map<string, double> >::const_iterator user_iter = userToMovie.begin(); user_iter != userToMovie.end(); user_iter++)
	{
		count++;
		if(count % 100 == 0)
			cout<<"("<<count<<",\t"<<userToMovie.size()<<")"<<endl;

		similarUsers[user_iter->first] = topMatchesUsers(userToMovie, user_iter->first);
	}

	return similarUsers;
}



map<string, vector<pair<string, double> > > calculateSimilarMovies(map<string, map<long, double> >& movieToUser, map<long, double>& averageRatings, int n)
{
	map<string, vector<pair<string, double> > > similarMovies;

	int count = 0;
	for(map<string, map<long, double> >::const_iterator movie_iter = movieToUser.begin(); movie_iter != movieToUser.end(); movie_iter++)
	{
		count++;
		if(count % 100 == 0)
			cout<<"("<<count<<",\t"<<movieToUser.size()<<")"<<endl;

		similarMovies[movie_iter->first] = topMatchesMovies(movieToUser, movie_iter->first, averageRatings);
	}

	return similarMovies;
}


map<long, double> getAverageRatings(map<long, map<string, double> >& userToMovie)
{
	map<long, double> averageRatings;

	for(map<long, map<string, double> >::const_iterator it = userToMovie.begin(); it != userToMovie.end(); it++)
	{
		for(map<string, double>::const_iterator movie_iter = it->second.begin(); movie_iter != it->second.end(); movie_iter++)
			averageRatings[it->first] += movie_iter->second;

		averageRatings[it->first] /= it->second.size();
	}

	return averageRatings;
}

