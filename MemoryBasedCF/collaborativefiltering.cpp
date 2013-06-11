#include "collaborativefiltering.h"

// Using Declarations
using std::cin;
using std::cout;
using std::endl;
using std::sort;
using std::string;
using std::vector;
using std::ifstream;
using std::map;
using std::ofstream;
using std::sort;

// To compare
bool compare_scores_user(const std::pair<long, double>& one, const std::pair<long, double>& two)
{
	if(one.second == two.second)
		return (one.first > two.first);
	else
		return (one.second > two.second);
}

bool compare_scores_movie(const std::pair<std::string, double>& one, const std::pair<std::string, double>& two)
{
	if(one.second == two.second)
		return (one.first > two.first);
	else
		return (one.second > two.second);
}

// Used to Load the dataset
int loadDataSet(char *moviefile, char *ratingsfile, std::map<long, std::map<std::string, double> >& userToMovie)
{
	std::string currentLine;						 // To hold the entire currentline
	std::map<long, std::string> movieMap;			 // Maps from the movie id to their  title

	{	
		std::vector<std::string> splitData;				 // To hold the double values from the currentline
		// Build a map from movie_ids to their titles
		ifstream infile(moviefile);						 // Open the file for getting the input

		//Always test the file open.
		if(!infile) 
		{
		  cout<<"Error opening output file"<<endl;
		  return -1;
		}
		
		// keep on reading till we get to the end of the file
		while(std::getline(infile, currentLine))
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
		infile.open(ratingsfile);
		vector<double> splitDouble;
		
		// Always tes the file
		if(!infile)
		{
			cout<<"Error Opening File"<<endl;
			return -1;
		}

		while(std::getline(infile, currentLine))
		{
			// Split the data into subparts
			splitDouble= split(currentLine);	

			// Store the rating for the user and the movie	
			userToMovie[splitDouble[0]][movieMap[splitDouble[1]]] = splitDouble[2];
		}
	}

	return 0;
}

int saveToFile(char* filename, const std::map<long, std::vector<std::pair<std::string, double> > >& recommendations)
{
	ofstream outfile;
	outfile.open(filename);

	for(std::map<long, std::vector<std::pair<std::string, double> > >::const_iterator it = recommendations.begin(); it != recommendations.end(); it++)
	{
		outfile<<it->first<<":\n";
		for(int i = 0; i < it->second.size(); i++)
			outfile<<it->second[i].first<<" "<<it->second[i].second<<"\n";
		cout<<"\n";
	}
	
	outfile.close();	
}

std::map<std::string, std::map<long, double> > transformPrefs(const std::map<long, std::map<std::string, double> >& userToMovie)
{
	// TO be returned
	std::map<std::string, std::map<long, double> > movieToUser;

	// Transform from user to movie rating to movie to user rating
	for(std::map<long, std::map<std::string, double> >::const_iterator user_iter = userToMovie.begin(); user_iter != userToMovie.end(); user_iter++)
		for(std::map<std::string, double>::const_iterator movie_iter = user_iter->second.begin(); movie_iter !=user_iter->second.end(); movie_iter++)
			movieToUser[movie_iter->first][user_iter->first] = movie_iter->second;

	return movieToUser;
}


std::vector<std::pair<long, double> > topMatchesUsers(std::map<long, std::map<std::string, double> >& userToMovie, long user, int n)
{
	// To hold the users and the similarity scores
	std::vector<std::pair<long, double> > scores;

	// Used for holding the common movies to cacluate the similarity scores
	std::vector<double> one, two;
	double score = 0.0;

	// Go through all other users except for the user
	for(std::map<long, std::map<std::string, double> >::const_iterator it = userToMovie.begin(); it != userToMovie.end(); it++)
		if(it->first != user)
		{
			for(std::map<std::string, double>::const_iterator movie_iter = it->second.begin(); movie_iter != it->second.end(); movie_iter++)
				if(userToMovie[user].find(movie_iter->first) != userToMovie[user].end())
				{
					//Found Common Movie
					one.push_back(userToMovie[user][movie_iter->first]);
					two.push_back(movie_iter->second);
				}

			// Calculate the similarity score and add it to the vector
			score = pearsonCoefficient(one, two);
			scores.push_back(std::make_pair<long, double>(it->first, score));
		}

	// sort them in the descending order of scores
	std::sort(scores.begin(), scores.end(), compare_scores_user);
	
	// keep only the top n elements
	scores.resize(n);

	return scores;
}


std::vector<std::pair<std::string, double> > topMatchesMovies(std::map<std::string, std::map<long, double> >& movieToUser, std::string movie, int n)
{
	// To hold the users and the similarity scores
	std::vector<std::pair<std::string, double> > scores;

	// Used for holding the common movies to cacluate the similarity scores
	std::vector<double> one, two;
	double score = 0.0;

	// Go through all other users except for the user
	for(std::map<std::string, std::map<long, double> >::const_iterator movie_iter = movieToUser.begin(); movie_iter != movieToUser.end(); movie_iter++)
		if(movie_iter->first != movie)
		{
			for(std::map<long, double>::const_iterator user_iter = movie_iter->second.begin(); user_iter != movie_iter->second.end(); user_iter++)
				if(movieToUser[movie].find(user_iter->first) != movieToUser[movie].end())
				{
					//Found Common Movie
					one.push_back(movieToUser[movie][user_iter->first]);
					two.push_back(user_iter->second);
				}

			// Calculate the similarity score and add it to the vector
			score = pearsonCoefficient(one, two);
			scores.push_back(std::make_pair<std::string, double>(movie_iter->first, score));
		}

	// sort them in the descending order of scores
	std::sort(scores.begin(), scores.end(), compare_scores_movie);
	
	// keep only the top n elements
	scores.resize(n);

	return scores;
}

std::vector<std::pair<long, double> > positiveCorrelationUsers(std::map<long, std::map<std::string, double> >& userToMovie, long user, int n)
{
	// To hold the users and the similarity scores
	std::vector<std::pair<long, double> > scores;
	
	// Used for holding the common movies to cacluate the similarity scores
	std::vector<double> one, two;
	double score = 0.0;

	// Go through all other users except for the user
	for(std::map<long, std::map<std::string, double> >::const_iterator it = userToMovie.begin(); it != userToMovie.end(); it++)
		if(it->first != user)
		{
			for(std::map<std::string, double>::const_iterator movie_iter = it->second.begin(); movie_iter != it->second.end(); movie_iter++)
				if(userToMovie[user].find(movie_iter->first) != userToMovie[user].end())
				{
					//Found Common Movie
					one.push_back(userToMovie[user][movie_iter->first]);
					two.push_back(movie_iter->second);
				}

			// Calculate the similarity score and add it to the vector if it is greater than zero
			score = pearsonCoefficient(one, two);
			if(score > 0.0)
				scores.push_back(std::make_pair<long, double>(it->first, score));
		}

	// sort them in the descending order of scores
	std::sort(scores.begin(), scores.end(), compare_scores_user);
	
	// keep only the top n elements
	scores.resize(n);

	return scores;
}

// Get the Top N recommendations
std::vector<std::pair<std::string, double> > getRecommendations(std::map<long, std::map<std::string, double> >& userToMovie, long user)
{
	//Create maps to hold the total of the weighted ratings and the total of the similarity scores
	std::map<std::string, double> totals;
	std::map<std::string, double> similaritySum;
	
	// Used for holding the common movies to cacluate the similarity scores
	std::vector<double> one, two;
	double score = 0.0;

	for(std::map<long, std::map<std::string, double> >::const_iterator it = userToMovie.begin(); it != userToMovie.end(); it++)
		if(it->first != user)
		{
			for(std::map<std::string, double>::const_iterator movie_iter = it->second.begin(); movie_iter != it->second.end(); movie_iter++)
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
				for(std::map<std::string, double>::const_iterator movie_iter = it->second.begin(); movie_iter != it->second.end(); movie_iter++)
					if(userToMovie[user].find(movie_iter->first) == userToMovie[user].end() || userToMovie[user][movie_iter->first] == 0)
					{
						totals[movie_iter->first] += movie_iter->second * score;
						similaritySum[movie_iter->first] += score;
					}
			}
		}

	// To hold the top recommendations
	std::vector<std::pair<std::string, double> > recommendations;

	// for each movie in totals calculate their normalized scores
	for(std::map<std::string, double>::iterator it = totals.begin(); it != totals.end(); it++)
		recommendations.push_back(std::make_pair<std::string, double>(it->first, it->second/similaritySum[it->first]));
	
	// Sort them according to the predicted score
	std::sort(recommendations.begin(), recommendations.end(), compare_scores_movie);

	return recommendations;
}


std::map<long, std::vector<std::pair<std::string, double> > > getRecommendationsUserBased(std::map<long, std::map<std::string, double> >& userToMovie)
{
	std::map<long, std::vector<std::pair<std::string, double> > > recommendedItemsUserBased;

	for(std::map<long, std::map<std::string, double> >::const_iterator user_iter = userToMovie.begin(); user_iter != userToMovie.end(); user_iter++)
	{
		//std::cout<<"User id: "<<user_iter->first<<std::endl;
		recommendedItemsUserBased[user_iter->first] = getRecommendations(userToMovie, user_iter->first);
	}

	return recommendedItemsUserBased;
}


std::map<long, std::vector<std::pair<std::string, double> > > getRecommendationsItemBased(std::map<std::string, std::map<long, double> >& movieToUser)
{
	std::map<long, std::vector<std::pair<std::string, double> > > recommendedItemsItemBased;

	for(std::map<std::string, std::map<long, double> >::const_iterator movie_iter = movieToUser.begin(); movie_iter != movieToUser.end(); movie_iter++)
	{
		//std::cout<<"User id: "<<user_iter->first<<std::endl;
		recommendedItemsUserBased[user_iter->first] = getRecommendations(userToMovie, user_iter->first);
	}
}

std::map<long, std::vector<std::pair<long, double> > > calculateSimilarUsers(std::map<long, std::map<std::string, double> >& userToMovie, int n)
{
	std::map<long, std::vector<std::pair<long, double> > > similarUsers;

	int count = 0;
	for(std::map<long, std::map<std::string, double> >::const_iterator user_iter = userToMovie.begin(); user_iter != userToMovie.end(); user_iter++)
	{
		count++;
		if(count % 100 == 0)
			cout<<"("<<count<<",\t"<<userToMovie.size()<<")"<<endl;

		similarUsers[user_iter->first] = topMatchesUsers(userToMovie, user_iter->first);
	}

	return similarUsers;
}



std::map<std::string, std::vector<std::pair<std::string, double> > > calculateSimilarMovies(std::map<std::string, std::map<long, double> >& movieToUser, int n)
{
	std::map<std::string, std::vector<std::pair<std::string, double> > > similarItems;

	int count = 0;
	for(std::map<std::string, std::map<long, double> >::const_iterator movie_iter = movieToUser.begin(); movie_iter != movieToUser.end(); movie_iter++)
	{
		count++;
		if(count % 100 == 0)
			cout<<"("<<count<<",\t"<<movieToUser.size()<<")"<<endl;

		
		similarItems[movie_iter->first] = topMatchesMovies(movieToUser, movie_iter->first);
	}

	return similarItems;
}

/*

def getRecommendedItems(prefs, itemMatch, user):
	userRatings = prefs[user]
	scores = {}
	totalSim = {}

	# Loop over items rated by this user
	for (item, rating) in userRatings.items():
		# Loop over items similar to this one
		for (similarity, item2) in itemMatch[item]:
			# Ignore if this user has already rated this item
			if item2 in userRatings:
				continue
			
			# Weighted sum of rating times similarity
			scores.setdefault(item2, 0)
			scores[item2] += similarity * rating
			
			# Sum of all the similarities
			totalSim.setdefault(item2, 0)
			totalSim[item2] += similarity

	# Divide each total score by total weighting to get an average
	rankings=[(score/totalSim[item],item) for item,score in scores.items()]
	
	# Return the rankings from highest to lowest
	rankings.sort()
	rankings.reverse()
	return rankings
*/