#include "constants.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>

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
using std::ostringstream;

// Converts anything to a string
template<class T> string to_string(T n)
{
    ostringstream os;
    os << n;
    return os.str();
}

// Read only the top K_RANK singular values into the singularMatrix
int readSingularValues(vector<vector<double> >& singularMatrix, const string& filename = "Outputsingular_values")
{
    // Load the singular Values and create the singular matrix
    ifstream infile;
    infile.open(filename.c_str());

    //Always test the file open.
    if(!infile) 
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
        singularMatrix[count][count] = temp;
        count++;
    }

    infile.clear();
    infile.close();
    
    cout << "Dimensions of the singular Matrix: " << singularMatrix.size() << ", " << singularMatrix[0].size() << "\n";
    
}

// Get the number of lines from the file to get the number of users
int getNumberLines(const string& filename = "OutputU.0_1_of_1")
{
    // Load the singular Values and create the singular matrix
    ifstream infile;
    infile.open(filename.c_str());

    //Always test the file open.
    if(!infile) 
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

// Read the uMatrix and the vMatrix from file
int readMatrices(vector<vector<double> >& uMatrix, vector<vector<double> >& vMatrix)
{
    double temp = 0;
    int count = 0;

    for(int i =0; i < NUM_EIGEN_VALUES; ++i)
    {
        ifstream infileU, infileV;
        infileU.open(("OutputU." + to_string(i) + "_1_of_1").c_str());
        infileV.open(("OutputV." + to_string(i) + "_1_of_1").c_str());

        count = 0;
        while(infileU >> temp)
            uMatrix[count++][i] = temp;

        count = 0;
        while(infileV >> temp)
            vMatrix[count++][i] = temp;

        infileU.close();
        infileV.close();
    }

}

void calculateOutput(vector<vector<double> >& uMatrix, vector<vector<double> >& vMatrix, vector<vector<double> >& singularMatrix, vector<vector<double> >& output)
{
    // TODO
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

    // Read the UMatrix and the VMatrix
    errorVal = readMatrices(uMatrix, vMatrix);

    // Used to store the output result
    vector<vector<double> > output(numberOfUsers, vector<double>(NUM_FEATURES));

    // Calculate the Output vector
    calculateOutput(uMatrix, vMatrix, singularMatrix, output);

    // Save the output file to be used by kmeans
    saveOutput(output, "kmeansinput.txt");

    return 0;
}