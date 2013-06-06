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

/*
int main()
{
	vector<double> one, two;

	one.push_back(2.5);
	one.push_back(3.5);
	one.push_back(3.0);
	one.push_back(3.5);
	one.push_back(2.5);
	one.push_back(3.0);

	two.push_back(3.0);
	two.push_back(3.5);
	two.push_back(1.5);
	two.push_back(5.0);
	two.push_back(3.5);
	two.push_back(3.0);

	
	cout<<"Distance: "<<calcPearson(one, two)<<endl;
	
	return 0;
}
*/

int main()
{
	// Create a map from the user id to the ratingMatrix
	vector<vector<double> > ratingMatrix;

	// Get the ratings into the map from the file
	int errorVal =  getRatingMatrix("ratings_with_id.txt", ratingMatrix);
	
	cout<<"RatingMatrix Dimensions: "<<ratingMatrix.size()<<", "<<ratingMatrix[0].size()<<endl;
	
	map<long, int> userMap;
	vector<long> userVector(ratingMatrix.size());

	for(int i = 0; i <ratingMatrix.size(); i++)
	{
		userMap[ratingMatrix[i][0]] = i;
		userVector[i] = ratingMatrix[i][0];
	}
	
	// Randomly shuffle the userVector
	vector<long >::iterator newBegin = random_unique(userVector.begin(), userVector.end(), NUM_FEATURES);
	int zeroCount = 0;
	vector<vector<double> > pearsonDistances;
	// For each user in the matrix calculate the canberra distance with the NUM_FEATURES randomly selected users
	for(int i = 0; i < ratingMatrix.size(); i++)
	{

		// This will hold the Canberra distances for the current user
		vector<double> distance(NUM_FEATURES + 1);

		// The first entry holds the userid which is taken from the ratingMatrix
		distance[0] = ratingMatrix[i][0];
		
		for(int j = 0; j < NUM_FEATURES; j++)
		{
			// Call the canberra distance function
			//cout<<"Distance between "<<i<<" and "<<userMap[userVector[j]]<<endl;

			distance[j + 1] = calcPearson(vector<double>(ratingMatrix[i].begin() + 1, ratingMatrix[i].end()), vector<double>(ratingMatrix[userMap[userVector[j]]].begin() + 1, ratingMatrix[userMap[userVector[j]]].end()));
			
			if(distance[j + 1] == 0)
				zeroCount++;

			//cout<<"Distance:"<<distance[j + 1]<<endl;
		}
		
		// Add the distance to the Canberra Distance Matrix
		pearsonDistances.push_back(distance);
	}
	
	cout<<"Dimensions of the Pearson Distance Matrix: "<<pearsonDistances.size()<<", "<<pearsonDistances[0].size()<<endl;
	
	// Save it to the file
	ofstream outfile;
	outfile.open("SVDInput.txt");
	for(int i = 0; i < pearsonDistances.size(); i++)
	{
		for(int j = 1; j < pearsonDistances[i].size(); j++)
			outfile<<i + 1<<" "<<j<<" "<<pearsonDistances[i][j]<<endl;
	}

	outfile.close();
	
	/*
	cout<<"Order of the users selected:"<<endl;
	for(int i = 0; i < pearsonDistances.size(); i++)
		cout<<pearsonDistances[i][0]<<" ";
	cout<<endl;

	cout<<"Order of Random Users:"<<endl;
	for(int i = 0; i < NUM_FEATURES; i++)
		cout<<userVector[i]<<" ";

	cout<<endl;	
	*/

	cout<<"Zero Count:"<<zeroCount<<endl;

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
