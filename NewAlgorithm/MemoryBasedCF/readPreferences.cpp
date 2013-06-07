#include "readPreferences.h"

// Used to Load the dataset
int loadDataSet(char *moviefile, char *ratingsfile, std::map<std::map<long, std::string> >& userToMovie)
{
	// Build a map from movie_ids to their titles
	ifstream infile;
	infile.open(moviefile);

	ifstream myfile(filename);						 // Open the file for getting the input
    std::string currentLine;						 // To hold the entire currentline
    std::vector<double> splitDouble;				 // To hold the double values from the currentline
	int userCount = 0;								 // To keep track of the number users

    //Always test the file open.
    if(!myfile) 
    {
      cout<<"Error opening output file"<<endl;
      return -1;
    }

}

int getRatingMatrix(const char* filename, vector<vector<double> >& ratingMatrix)
{
	ifstream myfile(filename);						 // Open the file for getting the input
    std::string currentLine;						 // To hold the entire currentline
    std::vector<double> splitDouble;				 // To hold the double values from the currentline
	int userCount = 0;								 // To keep track of the number users
	map<int, long> userMap;							 // To Map from the ratingMatrix Row to the actual Userid

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
		
		// Create a map from user_id to index the rating matrix
		if(userMap.find(userCount) == userMap.end())
		{
			userMap[userCount] = splitDouble[0];
			userCount++;
		}
		
		// Add it to the ratingMatrix
