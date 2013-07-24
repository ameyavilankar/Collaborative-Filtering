#include "kmeansinput.h"
#include "to_string.h"

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
using Eigen::MatrixXd;

// Read only the top K_RANK singular values into the singularMatrix
int readSingularValues(MatrixXd& singularMatrix, const string& filename = "Outputsingular_values")
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
        singularMatrix(count, count) = temp;
        count++;
    }

    infile.clear();
    infile.close();
    
    cout << "Dimensions of the singular Matrix: " << singularMatrix.rows() << ", " << singularMatrix.cols() << "\n";

    // return success
    return 0;
    
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
int readMatrices(MatrixXd& uMatrix, MatrixXd& vMatrix, size_t rank)
{
    double temp = 0;
    int  ulineNumber = 0;
    int vlineNumber = 0;
    string splitLine;
    vector<double> splitDouble;

    cout << "uMatrix: " << uMatrix.rows() << ", " << uMatrix.cols() << "\n";
    cout << "vMatrix: " << vMatrix.rows() << ", " << vMatrix.cols() << "\n";
    
    for(int i = 0; i < rank; ++i)
    {
	cout << "Reading in " << i << "\n ";
        ifstream infileU, infileV;
        infileU.open(("Output.U." + to_string(i) + "_1_of_1").c_str());
        infileV.open(("Output.V." + to_string(i) + "_1_of_1").c_str());
	
	ulineNumber = 0;
        while(std::getline(infileU, splitLine))
	{
	    splitDouble = split(splitLine);
	    //cout << ulineNumber << " , " << i << " " << splitDouble[1] << "\n";
	    uMatrix(ulineNumber++, i) = splitDouble[1];
	}
	
	vlineNumber = 0;
        while(std::getline(infileV, splitLine))
	{
	    splitDouble = split(splitLine);
	    //cout << vlineNumber  << " , " << i << " " << splitDouble[1] << "\n";
            vMatrix(vlineNumber++, i) = splitDouble[1];
	}
	
        infileU.close();
        infileV.close();
    }
    
    return 0;   
}

void calculateOutput(MatrixXd& uMatrix, MatrixXd& vMatrix, MatrixXd& singularMatrix, MatrixXd& output)
{
    // Calculate the output
    MatrixXd left = (uMatrix * singularMatrix);

    output = left * vMatrix.transpose();
}


int saveOutput(MatrixXd& output, const string& filename = "kmeansinput.txt")
{
    // Save the Output result to file.
    ofstream outfile;
    outfile.open(filename.c_str());
    
    if(!outfile)
    {
	cout << "Writing kmeansinput failed!!!\n";
	return -1;
    }
    
    for(int i = 0; i < output.rows(); i++)
    {
	// Output the id to the file
	outfile << i << " ";

	for(int j = 0; j < output.cols(); j++)
	    outfile << output(i, j) << " ";
	
	outfile << "\n";

    }

    outfile.close();
    
    // return sucess
    return 0;
}

int calculate_kmeans_input(size_t rank)
{
    std::cout << "Reading the Singular Values into Eigen Matrix...\n";
    // create a matrix of vectors
    MatrixXd singularMatrix =  MatrixXd::Zero(rank, rank);
    
    int errorVal = readSingularValues(singularMatrix, "Output.singular_values");
    if(errorVal != 0)
        return errorVal;
     
    std::cout << "Calculating the Number of users..\n";
    
    // Calculate the number of lines the UMatrix file to get the number of users
    int numberOfUsers = getNumberLines("Output.U.0_1_of_1");
    if(numberOfUsers == -1)
        return numberOfUsers;

    // Create the UMatrix and VMatrix
    MatrixXd uMatrix = MatrixXd::Zero(numberOfUsers, rank);
    MatrixXd vMatrix = MatrixXd::Zero(NUM_FEATURES, rank);
    
    std::cout << "Reading in the UMatrix and VMatrix...\n";
    
    // Read the UMatrix and the VMatrix
    errorVal = readMatrices(uMatrix, vMatrix, rank);
    if(errorVal != 0)
        return errorVal;

    // Used to store the output result
    MatrixXd output = MatrixXd::Zero(numberOfUsers, NUM_FEATURES);

    std::cout << "Calculating the Output Matrix...\n";
    // Calculate the Output vector
    calculateOutput(uMatrix, vMatrix, singularMatrix, output);
    
    std::cout << "Saving the Output to file...\n";
    // Save the output file to be used by kmeans
    errorVal = saveOutput(output, "kmeansinput.txt");
    
    if(errorVal != 0)
        return errorVal;

    return 0;
}
