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
	
	return 0;
}

