#include "main.h"
#include "readMatrix.cpp"
#include "canberra.cpp"

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

const int NUM_FEATURES = 100;

int main()
{
	// Create a map from the user id to the ratingMatrix
	vector<vector<double> > ratingMatrix;

	// Get the ratings into the map from the file
	int errorVal =  getRatingMatrix("ratings_with_id.txt", ratingMatrix);
	
	//cout<<"RatingMatrix Dimensions: "<<ratingMatrix.size()<<", "<<ratingMatrix[0].size();
	
	
	// Randomly Select the NUM_FEATURES Users from the existing set and get them to the front inplace
	/*
	cout<<"Before Random Shuffling:"<<endl;
	for(int i = 0; i < NUM_FEATURES; i++)
		cout<<ratingMatrix[i][0]<<" ";
	cout<<endl<<endl;
	*/

	vector<vector<double> >::iterator newBegin = random_unique(ratingMatrix.begin(), ratingMatrix.end(), NUM_FEATURES);
	
	/*
	cout<<"After Random Shuffling"<<endl;
	for(int i = 0; i < NUM_FEATURES; i++)
		cout<<ratingMatrix[i][0]<<" ";
	cout<<endl<<endl;
	*/
	// Create a Canberra Distance Matrix of size Number of users X NUM_FEATURES
	
	vector<vector<double> > canberraDistances;
	
	// For each user in the matrix calculate the canberra distance with the NUM_FEATURES randomly selected users
	for(int i = 0; i < ratingMatrix.size(); i++)
	{
		//cout<<"Distance for user: "<<i<<endl;

		// This will hold the Canberra distances for the current user
		vector<double> distance(NUM_FEATURES + 1);

		// The first entry holds the userid which is taken from the ratingMatrix
		distance[0] = ratingMatrix[i][0];

		for(int j = 0; j < NUM_FEATURES; j++)
		{
			//cout<<j<<" ";

			// Call the canberra distance function
			distance[j + 1] = calculate_canberradist(vector<double>(ratingMatrix[i].begin() + 1, ratingMatrix[i].end()), vector<double>(ratingMatrix[j].begin() + 1, ratingMatrix[j].end()));
		}
		
		// Add the distance to the Canberra Distance Matrix
		canberraDistances.push_back(distance);
	}
	
	//cout<<"Dimensions of the Canberra Distance Matrix: "<<canberraDistances.size()<<", "<<canberraDistances[0].size()<<endl;
	
	for(int i = 0; i < canberraDistances.size(); i++)
	{
		for(int j = 1; j < canberraDistances[i].size(); j++)
			cout<<i + 1<<" "<<j<<" "<<canberraDistances[i][j]<<endl;
	}

	cout<<"Order of the users selected:"<<endl;
	for(int i = 0; i < canberraDistances.size();i++)
		cout<<canberraDistances[i][0]<<endl;
	
	return 0; 
}


/*
	TEST CODE FOR RANDOM UNIQUE
	vector<double> values;
	values.push_back(2);
	values.push_back(3);
	values.push_back(4);
	values.push_back(5);
	values.push_back(6);
	values.push_back(7);

	for(vector<double>::iterator it = values.begin(); it != values.end(); it++)
		cout<<(*it)<<" ";
	cout<<endl;

	vector<double>::iterator newBegin = random_unique(values.begin(), values.end(), values.size() - 2);

	for(vector<double>::iterator it = values.begin(); it != values.end(); it++)
		cout<<(*it)<<" ";
	cout<<endl;
*/
