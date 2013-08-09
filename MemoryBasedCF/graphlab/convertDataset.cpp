#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

inline bool isColon(char c)
{
	return c == ':';
}

// Convert the string to its double equivalent
inline double getDouble(std::string toConvert)
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
		while (i != s.size() && isColon(s[i]))
			++i;

		// find end of next word
		string_size j = i;
		// invariant: none of the characters in range `['original `j', current `j)' is a space
		while (j != s.size() && !isColon(s[j]))
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

	/*
	 for(int i = 0; i < returnDouble.size(); i++)
	 	cout<<returnDouble[i]<<" ";
	 cout<<endl;
	*/

	return returnDouble;
}

int main(int argc, char const *argv[])
{
	vector<double> splitData;
	ifstream infile("ratings_with_id.txt");
	ofstream outfile("inputfile.txt");
	std::string currentLine;

	//Always test the file open.
	if(!infile) 
	{
	  cout<<"Error opening output file"<<endl;
	  return -1;
	}
	
	// keep on reading till we get to the end of the file
	while(getline(infile, currentLine))
	{
		splitData = split(currentLine);
		
		for(int i = 0; i < splitData.size() - 1; ++i)
			outfile << splitData[i] << " ";

		outfile << "\n";
	}
	
	outfile.close();
	infile.close();

	return 0;
}
