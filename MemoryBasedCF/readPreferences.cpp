#include "readPreferences.h"

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


// Used to Load the dataset
int loadDataSet(char *moviefile, char *ratingsfile, std::map<std::map<long, std::string> >& userToMovie)
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
			cout<<"Size: "<<splitData<<endl;

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

