#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

bool call_kmeans(const std::string& mpi_args, const std::string& filename,
    const std::string& kmeans_dir, const size_t num_clusters,
    const std::string& args)
{
    // To hold the entire funtion call
    std::stringstream strm;

    // Add only if to be run distributed using MPI
    if(mpi_args.length() > 0)
        strm << "mpiexec " << mpi_args << " ";

    // Build up the function call
    strm << kmeans_dir << "kmeans ";
    strm << " --data=" << filename /*<< ".compressed"*/;
    strm << " --clusters=" << num_clusters;
    strm << " --output-data=" << filename << ".result";
    strm << "--output-clusters=cluster" << filename;
    strm << " --id=1";
    strm << " " << args;

    // Call the function using the built string
    std::cout << "CALLING >" << strm.str() << std::endl;
    int sys_ret = system(strm.str().c_str());
    
    // Check if failed
    if (sys_ret != 0)
    {
        std::cout << "system call fails" << std::endl;
        return false;
    }

    return true;
}


int splitDatasets(int& numberOfUsers)
{
    // Create a map from the user id to the ratingMatrix
    vector<vector<double> > ratingMatrix;

    cout<<"Getting the RatingMatrix...\n";
    // Get the ratings into the map from the file
    int errorVal =  getRatingMatrix("ratings_with_id.txt", ratingMatrix);
    cout<<"RatingMatrix Dimensions: "<<ratingMatrix.size()<<", "<<ratingMatrix[0].size()<<endl;
    
    // Initialize random seed:
    srand (time(NULL));
    double randomNum = 0;

    std::cout<<"Splitting the Dataset into training and testing sets...\n";
    ofstream outTest, outTrain;
    outTest.open("test.txt");
    outTrain.open("train.txt");

    int trainCount = 0;
    int testCount = 0;
    for(int i = 0; i < ratingMatrix.size(); i++)
    {
        randomNum = ((double) rand() / (RAND_MAX));
        if(randomNum < 0.75)
        {
            for(int j = 0; j < ratingMatrix[i].size(); j++)
                outTrain << ratingMatrix[i][j] << " ";
            outTrain << "\n";

            trainCount++;
        }
        else
        {
            for(int j = 0; j < ratingMatrix[i].size(); j++)
                outTest << ratingMatrix[i][j] << " ";
            
            outTest << "\n";            
            testCount++;
        }
    }

        outTest.close();
        outTrain.close();

        std::cout << "Number of Users in Training Set: " << trainCount << "\n";
        std::cout << "Number of Users in Testing Set: " << testCount << "\n";
        std::cout << "Total Number of Users: " << ratingMatrix.size() << "\n";

        return 0;
}


int findK(const std::string& mpi_args,
    const std::string& kmeans_dir,
    const std::string& other)
{
    int numberOfUsers = 0;
    int errorVal = splitDatasets(numberOfUsers);
    if(errorVal != 0)
        return errorVal;

    std::cout << "Number of Users: " << numberOfUsers << "\n";

    // Find a way to get the upper limit
    int upperLimit = sqrt(numberOfUsers);

    for(int k = 2; k < upperLimit; k++)
    {
        std::cout << "For K = " << k << "\n";
        // std::cout << "Running Kmeans on the Training Set..\n";
        if (call_kmeans(mpi_args, "train.txt", kmeans_dir, k, other_args) == false)
            return EXIT_FAILURE;

        // run kmeans
        // std::cout << "Running Kmeans on the Testing Set..\n";
        if (call_kmeans(mpi_args, "test.txt", kmeans_dir, k, other_args) == false)
            return EXIT_FAILURE;


        // std::cout << "Geting the userToClusterMap and the clusterToUserMap for the Training and the Testing Sets...\n";
        map<long, int> UTCTestMap, UTCTrainMap;
        map<int, vector<long> > CTUTestMap, CTUTrainMap;
        errorVal = getUserToClusterMap("train.txt.result_1_of_1", UTCTrainMap, CTUTrainMap);
        if(errorVal != 0)
            return errorVal;
        
        errorVal = getUserToClusterMap("train.txt.result_1_of_1", UTCTestMap, CTUTestMap);
        if(errorVal != 0)
            return errorVal;

        std::cout << "No. of Training users: " << UTCTrainMap.size() << "\n";
        std::cout << "No. of Testing Users: " << UTCTestMap.size() << "\n";
        std::cout << "No. of Clusters: " << CTUTrainMap.size() << " " << CTUTestMap.size() << "\n";

        std::cout << "Geting the Cluster Centers for Training set..\n";
        vector<vector<double> > clusterCentersTrain;
        errorVal = getClusterCenters("clustertrain.txt", clusterCentersTrain);
        if(errorVal != 0)
            return errorVal;

        cout << "Cluster Dimensions: " << clusterCentersTrain.size() << ", " << clusterCentersTrain[0].size() << "\n";


    }
    
}