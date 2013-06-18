#include "main.h"
#include "readMatrix.cpp"
#include "canberra.cpp"
#include "math.h"
#include "preprocess.h"
#include "preprocess.cpp"

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

inline void getUserMapAndVector(const std::vector<std::vector<double> >& ratingMatrix, std::map<long, int>& userMap, std::vector<long>& userVector)
{
	for(int i = 0; i <ratingMatrix.size(); i++)
	{
		userMap[ratingMatrix[i][0]] = i;
		userVector[i] = ratingMatrix[i][0];
	}
}


int main()
{
	// Create a map from the user id to the ratingMatrix
	vector<vector<double> > ratingMatrix;

	cout<<"Getting the RatingMatrix...\n";
	// Get the ratings into the map from the file
	int errorVal =  getRatingMatrix("ratings_with_id.txt", ratingMatrix);
	cout<<"RatingMatrix Dimensions: "<<ratingMatrix.size()<<", "<<ratingMatrix[0].size()<<endl;
	
	
	cout<<"Calculating the userMap and the userVector...\n";
	// Get the userMap and the userVector
	map<long, int> userMap;
	vector<long> userVector(ratingMatrix.size());
	getUserMapAndVector(ratingMatrix, userMap, userVector);
	
	// Randomly shuffle the userVector
	vector<long >::iterator newBegin = random_unique(userVector.begin(), userVector.end(), NUM_FEATURES);
	
    
	cout<<"Calculating and Processing the distance Matrix...\n";
	// Calculate the cosine distances to the R randomly selected users
	vector<vector<double> > cosineDistances = getCosineMatrix(ratingMatrix, userMap, userVector);
	cout<<"Dimensions of the Cosine Distance Matrix: "<<cosineDistances.size()<<", "<<cosineDistances[0].size()<<endl;
	
	// Preprocess the matrix
	preprocessMatrix(cosineDistances);

	cout<<"Saving the Matrix to the file...\n";
	// Save it to the file
	ofstream outfile;
	outfile.open("SVDInput.txt");
	for(int i = 0; i < cosineDistances.size(); i++)
	{
		for(int j = 1; j < cosineDistances[i].size(); j++)
			outfile<<i + 1<<" "<<j<<" "<<cosineDistances[i][j]<<endl;
	}

	outfile.close();
	
	return 0;
}
