#include "preprocess.h"

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::ifstream;
using std::map;
using std::ofstream;


vector<vector<double> > getCosineMatrix(const vector<vector<double> >& ratingMatrix, map<long, int> userMap, vector<long> userVector)
{
    vector<vector<double> > cosineDistances;

    // For each user in the matrix calculate the cosine distance with the NUM_FEATURES randomly selected users
    for(int i = 0; i < ratingMatrix.size(); i++)
    {
	// This will hold the cosine distances for the current user
	vector<double> distance(NUM_FEATURES + 1);

	// The first entry holds the userid which is taken from the ratingMatrix
	distance[0] = ratingMatrix[i][0];
	
	// Calculate the cosine similarity with the randomly selected users		
	for(int j = 0; j < NUM_FEATURES; j++)
	{	
	    distance[j + 1]  = cosineSimilarity(vector<double>(ratingMatrix[i].begin() + 1, ratingMatrix[i].end()), vector<double>(ratingMatrix[userMap[userVector[j]]].begin() + 1, ratingMatrix[userMap[userVector[j]]].end()));
	}

	// Add the distance to the cosine Distance Matrix
	cosineDistances.push_back(distance);
    }

    return cosineDistances;
}

void normalizeMatrix(vector<vector<double> >& cosineDistances)
{	
    // Normalize along columns first, then normalize along rows
    normalizeAlongColumns(cosineDistances);
    normalizeAlongRows(cosineDistances);
}

void preprocessMatrix(vector<vector<double> >& cosineDistances)
{
    // Convert from the similarity to distances
    convertToDistances(cosineDistances);

    // Normalize the distances along the along the column and the rows
    normalizeMatrix(cosineDistances);
}
