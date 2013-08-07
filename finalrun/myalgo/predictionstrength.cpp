#include "predictionstrength.h"

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


int splitDatasets(std::map<long, std::vector<double> >& ratingMatrix)
{
    // Initialize random seed:
    srand (time(NULL));
    double randomNum = 0;

    std::cout<<"Splitting the Dataset into training and testing sets...\n";
    ofstream outTest, outTrain;
    outTest.open("test.txt");
    outTrain.open("train.txt");

    int trainCount = 0;
    int testCount = 0;
    for(std::map<long, std::vector<double> >::const_iterator it = ratingMatrix.begin(); it != ratingMatrix.end(); it++)
    {
        randomNum = ((double) rand() / (RAND_MAX));
        if(randomNum < 0.75)
        {
            outTrain << it->first << " ";
            for(int j = 0; j < it->second.size(); j++)
                outTrain << it->second[j] << " ";
            outTrain << "\n";

            trainCount++;
        }
        else
        {
            outTest << it->first << " ";
            for(int j = 0; j < it->second.size(); j++)
                outTest << it->second[j] << " ";
            
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

// Used to read the cluster centers and store them in clusterCenters
int getClusterCenters(const char* filename, std::map<int, std::vector<double> >& clusterCenters)
{
    // TODO
    std::ifstream myfile(filename);                       // Open the file for getting the input
    std::string currentLine;                         // To hold the entire currentline
    std::vector<double> splitDouble;                 // To hold the double values from the currentline
    
    //Always test the file open.
    if(!myfile) 
    {
      std::cout<<"Error opening output file"<<endl;
      return -1;
    }
    
    // Read till the end of the file
    while (std::getline (myfile, currentLine)) 
    {
        // Split the currentLine and only return the double parts
        splitDouble = split(currentLine);

        // TODO CHECKS for first element of the splitDouble
        clusterCenters[splitDouble[0]] = std::vector<double>(splitDouble.begin() + 1, splitDouble.end());
    }

    return 0;
}


int findK(const std::string& mpi_args, const std::string& kmeans_dir, const std::string& other_args, int& bestK)
{
     // Create a std::map from the user id to the ratingMatrix
    std::map<long, std::vector<double> > ratingMatrix;

    std::cout<<"Getting the RatingMatrix...\n";
    // Get the ratings into the std::map from the file
    int errorVal =  getRatingMatrix("ratings_with_id.txt", ratingMatrix);
    std::cout<<"RatingMatrix Dimensions: "<<ratingMatrix.size()<<", "<<ratingMatrix[ratingMatrix.begin()->first].size()<<endl;
    int numberOfUsers = ratingMatrix.size();

    errorVal = splitDatasets(ratingMatrix);
    if(errorVal != 0)
        return errorVal;

    std::cout << "Number of Users: " << numberOfUsers << "\n";

    // Find a way to get the upper limit
    int upperLimit = sqrt(numberOfUsers);

    std::map<int, double> strength;
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
        std::map<long, int> UTCTestMap, UTCTrainMap;
        std::map<int, std::vector<long> > CTUTestMap, CTUTrainMap;
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
        std::map<int, std::vector<double> > clusterCentersTrain;
        errorVal = getClusterCenters("clustertrain.txt", clusterCentersTrain);
        if(errorVal != 0)
            return errorVal;

        std::cout << "Cluster Dimensions: " << clusterCentersTrain.size() << ", " << clusterCentersTrain[1].size() << "\n";


        std::cout << "Finding the training cluster assignments for test users..\n";
        std::map<long, int> newClusters;
        for(std::map<long, std::vector<double> >::const_iterator user_it = ratingMatrix.begin(); user_it != ratingMatrix.end(); user_it++)
        {
            int trainCluster = 1;
            double minDistance = std::numeric_limits<double>::max();
            int distance = 0;

            for(std::map<int, std::vector<double> >::const_iterator cluster_it = clusterCentersTrain.begin(); cluster_it != clusterCentersTrain.end(); cluster_it++)
            {
                distance = (exp(-cosineSimilarity(user_it->second, cluster_it->second)));
                if(distance < minDistance)
                {
                    trainCluster = cluster_it->first;
                    minDistance = distance;
                }
            }

            newClusters[user_it->first] = trainCluster;

        }

        // To hold the cluster prediction strength
        std::map<int, double> clusterStrength;
        for(std::map<int, std::vector<long> >::const_iterator it = CTUTrainMap.begin(); it != CTUTrainMap.end(); it++)
        {
            int sameClusterCount = 0;

            // Find pairs of users that are in the same cluster
            for(int i = 0; i < it->second.size() - 1; i++)
                for(int j = i; j < it->second.size(); j++)
                {
                    if(newClusters[it->second[i]] == newClusters[it->second[j]])
                        sameClusterCount++;
                }

            // Find the proportion of the users that were in the same cluster for each cluster
            clusterStrength[it->first] = (double)sameClusterCount/it->second.size();
        }

        // Find the minimum prediction strength among all the clusters
        double predictionStrength = std::numeric_limits<double>::max();
        for(std::map<int, double>::const_iterator it = clusterStrength.begin(); it != clusterStrength.end(); it++)
        {
            if(it->second < predictionStrength)
                predictionStrength = it->second;
        }

        /*
        // Average Instead of Minimum strength
        double predictionStrength = 0;
        for(std::map<int, double>::const_iterator it = clusterStrength.begin(); it != clusterStrength.end(); it+)
        {
            predictionStrength += it->second;
        }

        predictionStrength /= clusterStrength.size();
        */

        strength[k] = predictionStrength;

    }
    
    // Find the number of clusters that resulted in the maximum prediction
    bestK = -1;
    double maxStrength = std::numeric_limits<double>::min();
    for(std::map<int, double>::const_iterator it = strength.begin(); it != strength.end(); it++)
        if(it->second > maxStrength)
        {
            maxStrength = it->second;
            bestK = it->first;
        }

    std::cout << "The Best K is: " << bestK << "\n";

    return 0;
}