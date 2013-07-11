#include "constants.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using std::count;
using std::cout;
using std::cin;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::getline;
using std::vector;
using std::string;
using std::istream_iterator;

int readSingularValues(vector<vector<double> >& singularMatrix, const string& filename = "Outputsingular_values")
{
    // Load the singular Values and create the singular matrix
    ifstream infile;
    infile.open(filename.c_str());

    //Always test the file open.
    if(!myfile) 
    {
        cout<<"Error opening output file"<<endl;
        return -1;
    }

    // Ignore the first line of the singular values file
    string ignoreLine;
    getline (infile, ignoreLine);

    // Append singular values to the vector
    double temp;
    int count = 0;
    while(infile >> temp && count <= K_RANK)
    {
        singularValues[count][count] = temp;
        count++;
    }

    infile.clear();
    infile.close();
    
    cout << "Dimensions of the singular Matrix: " << singularMatrix.size() << ", " << singularMatrix[0].size() << "\n";
    
}

int getNumberLines(const string& filename = "OutputU.0_1_of_1")
{
    // Load the singular Values and create the singular matrix
    ifstream infile;
    infile.open(filename.c_str());

    //Always test the file open.
    if(!myfile) 
    {
        cout<<"Error opening output file"<<endl;
        return -1;
    }

    // new lines will be skipped unless we stop it from happening:    
    infile.unsetf(std::ios_base::skipws);

    // count the newlines with an algorithm specialized for counting:
    unsigned line_count = std::count(istream_iterator<char>(infile), istream_iterator<char>(), '\n');

    cout << "Lines: " << line_count << "\n";
    return line_count;
}

void saveOutput(vector<vector<double> >& output, const string& filename = "kmeansinput.txt")
{
    // Save the Output result to file.
    ofstream outfile;
    outfile.open(filename.c_str());

    for(int i = 0; i < output.size(); ++i)
    {
        for(int j = 0; j < output[i].size(); ++j)
                outfile << output[i][j] << " ";
        
        outfile << "\n";
    }

    outfile.close();
}

int main()
{
    // create a matrix of vectors
    vector<vector<double> > singularMatrix(NUM_EIGEN_VALUES, vector<double>(NUM_EIGEN_VALUES));
    int errorVal = readSingularValues(singularMatrix, "Outputsingular_values");

    // Calculate the number of lines the UMatrix file to get the number of users
    int numberOfUsers = getNumberLines("OutputU.0_1_of_1");

    // Create the UMatrix and VMatrix
    vector<vector<double> > uMatrix(numberOfUsers, vector<double>(NUM_EIGEN_VALUES));
    vector<vector<double> > vMatrix(NUM_FEATURES, vector<double>(NUM_EIGEN_VALUES));

    for(int i = 0; i < NUM_EIGEN_VALUES; ++i)
    {
        ifstream infile;

        infile.open("OutputU." + string(i))
    }


    vector<vector<double> > output(numberOfUsers, vector<double>(NUM_FEATURES));

    // Calculate the Output vector
    
    
    // Save the output file to be used by kmeans
    saveOutput(output, "kmeansinput.txt");

    return 0;
}