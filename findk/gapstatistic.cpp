#include "gapstatistic.h"

/*
Description of Gap Statistic:
http://blog.echen.me/2011/03/19/counting-clusters/

For each i from 1 up to some maximum number of clusters,

1. Run a k-means algorithm on the original dataset to find i clusters, and sum the distance of all
points from their cluster mean. Call this sum the dispersion.

2. Generate a set of reference datasets (of the same size as the original). One simple way of
generating a reference dataset is to sample uniformly from the original dataset’s bounding 
rectangle; a more sophisticated approach is take into account the original dataset’s shape
by sampling, say, from a rectangle formed from the original dataset’s principal components.

3. Calculate the dispersion of each of these reference datasets, and take their mean.

4. Define the ith gap by: log(mean dispersion of reference datasets) - log(dispersion of original dataset).

Once we’ve calculated all the gaps (we can add confidence intervals as well;
see the original paper for the formula), we can select the number of clusters to be
the one that gives the maximum gap. (Sidenote: I view the gap statistic as a very
statistical-minded algorithm, since it compares the original dataset against a set of
reference “control” datasets.)

*/


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
    strm << " --output-data=" << filename << ".result_k" << num_clusters;
    strm << "--output-clusters=cluster" << filename << "_k" << num_clusters;
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

struct range
{
    double min;
    double max;

    range(double m1, double m2): min(m1), max(m2) {}
}; 

int generate_reference_datasets(std::map<long, std::vector<double> >& ratingMatrix, const std::string& filename, int N = 10)
{
    // Get the Number of dimensions
    int num_dimensions = ratingMatrix[ratingMatrix.begin()->first].size();

    // Calculate the min and the max ranges for each column
    std::vector<range> ranges;

    for(int i = 0; i < num_dimensions; i++)
    {
        double min = std::numeric_limits<double>::max();
        double max = std::numeric_limits<double>::min();

        for(std::map<long, std::vector<double> >::const_iterator it = ratingMatrix.begin(); it != ratingMatrix.end(); it++)
        {
            if(it->second[i] < min)
                min = it->second[i];

            if(it->second[i] > max)
                max = it->second[i];
        }

        ranges.push_back(range(min, max));
    }

    std::cout << "Number of Dimensions: " << ranges.size() << "\n";

    std::cout << "Generating Reference Datasets:\n";    
    for(int reference_num = 1; reference_num < N; reference_num++)
    {
        std::cout << "Generating Reference Dataser No: " << reference_num << "\n";

        // Open the reference file with proper name for writing
        std::ofstream outFile;
        std::string reference_filename = filename + to_string(reference_num);
        outFile.open(reference_filename.c_str());

        for(int i = 0; i < ratingMatrix.size(); i++)
        {
            // Output the id to the file
            outFile << i << " ";

            // Output the uniformally generated row to the fiel
            for(int j = 0; j < num_dimensions; j++)
            {
                // Generate a number uniformally in the min-max range for the current dimension
                double number = graphlab::random::uniform(ranges[j].min, ranges[j].max);
                outFile << number << " ";
            }
            
            // To start a new row
            outFile << "\n";
        }

        outFile.close();
    }

    return 0;
}


int calculate_dispersion(std::string filename, int k, double& dispersion, const std::string& mpi_args, const std::string& kmeans_dir, const std::string& other_args)
{
    // Create a std::map from the user id to the ratingMatrix
    std::map<long, std::vector<double> > ratingMatrix;

    // Read in the Original Dataset
    std::cout<<"Getting the RatingMatrix for " << filename << "...\n";
    int errorVal =  getRatingMatrix(filename.c_str(), ratingMatrix);
    if(errorVal != 0)
    {
        cout << "errorVal = " << errorVal << "\n";
        return EXIT_FAILURE;
    }

    int numberOfUsers = ratingMatrix.size();

    // Run kmeans on the dataset using the current K
    std::cout << "Running Kmeans on the Dataset: " << filename << " , for k = " << k << "\n";
    if (call_kmeans(mpi_args, filename, kmeans_dir, k, other_args) == false)
        return EXIT_FAILURE;

    // Reading in the mapping from users to clusters and users to clusters
    cout<<"Geting the userToClusterMap and the clusterToUserMap...\n";
    map<long, int> userToClusterMap;
    map<int, vector<long> > clusterToUserMap;
    std::string mapping_file = "ratings_with_id.txt.result_k" + to_string(k) + "_1_of_1";
    errorVal = getUserToClusterMap(mapping_file.c_str(), userToClusterMap, clusterToUserMap);
    if(errorVal != 0)
        return errorVal;
    cout << "No. of users: " << userToClusterMap.size() << "\n";
    cout << "No. of Clusters:" << clusterToUserMap.size() << "\n\n";

    // Reading in the cluster centers
    std::cout << "Geting the Cluster Centers for Actual dataset ..\n";
    std::map<int, std::vector<double> > clusterCenters;
    errorVal = getClusterCenters("clustertrain.txt", clusterCenters);
    if(errorVal != 0)
    {
        cout << "errorVal = " << errorVal << "\n";
        return EXIT_FAILURE;
    }

    std::cout << "Cluster Dimensions: " << clusterCenters.size() << ", " << clusterCenters[1].size() << "\n";
    dispersion = 0.0;
    for(std::map<int, std::vector<long> >::const_iterator cluster_it = clusterToUserMap.begin(); cluster_it != clusterToUserMap.end(); cluster_it++)
    {
        // Add the dispersion for each cluster to global value
        for(int i = 0; i < cluster_it->second.size(); i++)
            dispersion += (exp(-cosineSimilarity(ratingMatrix[cluster_it->second[i]], clusterCenters[cluster_it->first])));
    }

    return 0;
}

int findK(const std::string& mpi_args,const std::string& kmeans_dir, std::string& kmeansinput, const std::string& other_args, int& bestK, int num_ref_datasets = 10)
{
    // Create a std::map from the user id to the ratingMatrix
    std::map<long, std::vector<double> > ratingMatrix;

    // TODO: Remove this line
    kmeansinput = "ratings_with_id.txt";

    // Read in the Original Dataset
    std::cout<<"Getting the RatingMatrix...\n";
    int errorVal =  getRatingMatrix(kmeansinput.c_str(), ratingMatrix);
    if(errorVal != 0)
    {
        cout << "errorVal = " << errorVal << "\n";
        return EXIT_FAILURE;
    }

    std::cout << "RatingMatrix Dimensions: " << ratingMatrix.size() << ", " << ratingMatrix[ratingMatrix.begin()->first].size() << endl;
    int numberOfUsers = ratingMatrix.size();

    // TODO: // Generate a set of Reference datasets save them all to file.
    errorVal = generate_reference_datasets(ratingMatrix, kmeansinput, num_ref_datasets);
    if(errorVal != 0)
    {
        cout << "errorVal = " << errorVal << "\n";
        return EXIT_FAILURE;
    }

    // Find a way to get the upper limit
    int upperLimit = sqrt(numberOfUsers);

    // For arange of K values
    std::map<int, double> gapStatistic;
    for(int k = 2; k < upperLimit; k++)
    {

        std::cout << "Calculate dispersion for the original dataset and the current value of k = " << k << "\n";
        double original_dispersion = 0.0;

        errorVal = calculate_dispersion(kmeansinput, k, original_dispersion, mpi_args, kmeans_dir, other_args);
        if(errorVal != 0)
        {
            std::cout << "ErrorVal : " << errorVal << "\n";
            return EXIT_FAILURE;
        }

        std::cout << "Calculate dispersions for the reference datasets and the current value of k = " << k << "\n";
        std::map<int, double> reference_dispersions; 
        for(int i = 1; i <= num_ref_datasets; i++)
        {
            std::cout << i;
            double disp = 0.0;
            std::string filename = kmeansinput + "_ref" + to_string(i);
            
            errorVal = calculate_dispersion(filename, k, disp, mpi_args, kmeans_dir, other_args);
            if(errorVal != 0)
            {
                std::cout << "ErrorVal : " << errorVal << "\n";
                return EXIT_FAILURE;
            }

            reference_dispersions[i] = disp;
        }

        std::cout << "\n";
        std::cout << "Calculate the avg_reference_dispersion for k = " << k << "\n";
        double avg_reference_dispersion = 0.0;
        for(std::map<int, double>::const_iterator it = reference_dispersions.begin(); it != reference_dispersions.end(); it++)
            avg_reference_dispersion += it->second;

        avg_reference_dispersion /= reference_dispersions.size();
        
        // Define the kth gap by: log(mean dispersion of reference datasets) - log(dispersion of original dataset).
        gapStatistic[k] = (log(avg_reference_dispersion) - log(original_dispersion));
    }


    // Find the maximum gap statistic
    double max = std::numeric_limits<double>::min();
    bestK = -1;
    for(map<int, double>::const_iterator it = gapStatistic.begin(); it != gapStatistic.end(); it++)
        if(it->second > max)
        {
            bestK = it->first;
            max = it->second;
        }

    std::cout << "Best Value of K is: " << bestK << "\n";

    return 0;
}