#include "readMatrix.h"

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

// Function used to determine if the current character is a colon or not
inline bool isSpace(char c)
{
	return c == ' ';
}

// Convert the string to its double equivalent
double getDouble(std::string toConvert)
{
	return atof(toConvert.c_str());
}

// Funciton used to split the sentence into its individual parts
vector<double> split(const string& s)
{
	vector<string> returnString;

	typedef string::size_type string_size;
	string_size i = 0;

	// invariant: we have processed characters `['original value of `i', `i)'
	while (i != s.size()) 
	{
		// ignore leading blanks
		// invariant: characters in range `['original `i', current `i)' are all spaces
		while (i != s.size() && isSpace(s[i]))
			++i;

		// find end of next word
		string_size j = i;
		// invariant: none of the characters in range `['original `j', current `j)' is a space
		while (j != s.size() && !isSpace(s[j]))
			++j;

		// if we found some nonwhitespace characters
		if (i != j) 
		{
			// copy from `s' starting at `i' and taking `j' `\-' `i' chars
			returnString.push_back(s.substr(i, j - i));
			i = j;
		}
	}

	std::vector<double> returnDouble;
	std::transform(returnString.begin(), returnString.end(), std::back_inserter(returnDouble), getDouble);

	// for(int i = 0; i < returnDouble.size(); i++)
	// 	cout<<returnDouble[i]<<" ";
	// cout<<endl;

	return returnDouble;
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
		//ratingMatrix.push_back(vector<double>(splitDouble.begin() + 1, splitDouble.end()));
		ratingMatrix.push_back(vector<double>(splitDouble.begin(), splitDouble.end()));
    }

	cout<<"Maximum Number of Users: "<<ratingMatrix.size()<<endl;
	cout<<"Maximum Number of Movies:"<<ratingMatrix[0].size()<<endl;

	cout<<"Rating Matrix Size: "<<ratingMatrix.size()<<", "<<ratingMatrix[0].size()<<endl;
	
	/*
	for(int i = 0; i < ratingMatrix.size(); i++)
    {	
    	cout<<i<<" ";
    	for(int j = 0; j < ratingMatrix[i].size(); j++)
    		cout<<ratingMatrix[i][j]<<" ";
    	cout<<endl;
    }
    */

	return 0; 
}


template<class fwditer>fwditer random_unique(fwditer begin, fwditer end, size_t num_random)
{
	// Find the Number of Elements between begin and end
	size_t left = std::distance(begin, end);
	
	if(left < num_random || num_random < 0)
	{
		cout<<"Cannot generate enough Random Variables."<<endl;
		return begin;
	}
	
	// Seed for different random number everytime
	srand(time(0));

	// While we do not have num_random numbers
	while (num_random--)
	{
		// Set forwarditerator to the current begin, which is the end of the unique
		fwditer r = begin;
		
		// Advance the iterator randomly between begin() and end()
		std::advance(r, rand()%left);

		// Swap with begin and update the begin and left values
		std::swap(*begin, *r);
		++begin;
		 --left;
	}
	
	return begin;
}

/*
int main()
{
	vector<vector<double> > ratingMatrix;
	//int errorVal =  getRatingMatrix("ratings_with_id.txt", ratingMatrix);
	
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

	return 0; 
}
*/


