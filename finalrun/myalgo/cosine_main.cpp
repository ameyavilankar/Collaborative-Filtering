#include "cosine_main.h"

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

inline int readClusterCenterUsers(map<long, int>& clusterUsers, string filename)
{
    ifstream myfile(filename.c_str());						 // Open the file for getting the input
    std::string currentLine;						 // To hold the entire currentline
    std::vector<double> splitDouble;				 // To hold the double values from the currentline

    //Always test the file open.
    if(!myfile) 
    {
		cout<<"Error opening output file"<<endl;
		return -1;
    }
	
    // Read till the end of the file
    while (std::getline (myfile, currentLine)) 
    {
    	// Split the currentLine and only return the double parts
	splitDouble = split(currentLine);
			
	// add user to the cluster
	clusterUsers[splitDouble[1]] = splitDouble[0];
    }

    cout<<"Number of users selected from old cluster centers: "<<clusterUsers.size()<<"\n";

    return 0;
}

// Selects the cluster centers users to be among the R users and selects the remaining users randomly
void getRandomAndClusterUsers(vector<long>& userVector, map<long, int> clusterUsers)
{
    int count = 0;
    long temp  = 0;

    for(int i = 0; i < userVector.size(); i++)
    {
	// search for the user present in the cluster centers and move it to the front of the list
	if(clusterUsers.find(userVector[i]) != clusterUsers.end())
	{
	    cout<<"Match: "<<userVector[i]<<"\n";

	    // swap the two
	    temp = userVector[count];
	    userVector[count] = userVector[i];
	    userVector[i] = temp;
	    count++;
	}
    }

    cout<<"Count: "<<count<<" , ClusterUsers: "<<clusterUsers.size()<<"\n";

    // Randomly select the remaining number of users
    random_unique(userVector.begin() + count, userVector.end(), NUM_FEATURES - count);

    userVector.resize(NUM_FEATURES);
}

int generate_cosine_matrix()
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
    
    cout<<"Reading the clusterUsers...\n";
    map<long, int> clusterUsers;
    errorVal = readClusterCenterUsers(clusterUsers, "clusterUsers.txt");
    if(errorVal != 0)
	   return errorVal;

    // Randomly shuffle the userVector
    // getRandomAndClusterUsers(userVector, clusterUsers);

    // Randomly select the remaining number of users
    random_unique(userVector.begin() , userVector.end(), NUM_FEATURES);

    userVector.resize(NUM_FEATURES);
    
    /*
    cout<<"After shuffling top NUM_FEATURES: \n";
    for(int i = 0; i < NUM_FEATURES; i++)
	cout<<userVector[i]<<" ";
    cout<<endl;	

    cout<<"userVector.size(): "<<userVector.size()<<" , NUM_FEATURES: "<<NUM_FEATURES<<"\n";
   */ 

    cout<<"Calculating and Processing the distance Matrix...\n";
    // Calculate the cosine distances to the R randomly selected users
    vector<vector<double> > cosineDistances = getCosineMatrix(ratingMatrix, userMap, userVector);
    cout<<"Dimensions of the Cosine Distance Matrix: "<<cosineDistances.size()<<", "<<cosineDistances[0].size()<<endl;
    
    // Preprocess the matrix
    preprocessMatrix(cosineDistances);

    cout<<"Saving the Matrix to the file...\n";
    // Save it to the file
    ofstream outfile;
    outfile.open("Output");
    for(int i = 0; i < cosineDistances.size(); i++)
    {
	   for(int j = 1; j < cosineDistances[i].size(); j++)
	       outfile<<i + 1<<" "<<j<<" "<<cosineDistances[i][j]<<endl;
    }

    outfile.close();
    
    return 0;
}
