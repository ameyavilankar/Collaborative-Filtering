#include "main.h"

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
using std::pair;

int main()
{
	// Load the dataset into the map
	cout<<"Loading the Dataset...\n";
	std::map<long, std::map<std::string, double> > userToMovie;
	int errorval = loadDataSet("movies.dat", "ratings.dat", userToMovie);
	if(errorval != 0)
		return errorval;
	
	// USER-BASED CF
	cout<<"Starting the User Based CF...\n";
	cout<<"Getting the Recommended Items for each person...\n";
	map<long, vector<pair<string, double> > > recommendedItemsUserBased = getRecommendationsUserBased(userToMovie);

	cout<<"Getting the Top Similar Users for each Person...\n";
	map<long, vector<pair<long, double> > > topSimilarUsers = calculateSimilarUsers(userToMovie);

	// ITEM-BASED CF
	cout<<"Starting the Item Based CF...\n";
	cout<<"Transforming the map...";
	map<string, map<long, double> >movieToUser = transformPrefs(userToMovie);
	
	cout<<"Calculating the top similar Movies for each movie...\n";
	map<string, vector<pair<string, double> > > topSimilarMovies = calculateSimilarMovies(movieToUser);
	
	cout<<"Calculating the recommendations for each user...\n";
	map<long, vector<pair<string, double> > > recommendedItemsItemBased = getRecommendationsItemBased(userToMovie, topSimilarMovies);

	cout<<"Saving to files...\n";
	cout<<"Saving Recommendations UserBased...\n";
	saveRecommendations("recommendedItemsUserBased.txt", recommendedItemsUserBased);
	
	cout<<"Saving Top Similar Users...\n";
	saveRecommendations("topSimilarUsers.txt", topSimilarUsers);

	cout<<"Saving Top Similar Movies...\n";
	saveRecommendations("topSimilarMovies", topSimilarMovies);

	cout<<"Saving Recommendations ItemBased...\n";
	saveRecommendations("recommendedItemsItemBased.txt", recommendedItemsItemBased);

	// Metrics

	return 0;
}
