For each i from 1 up to some maximum number of clusters,

Run a k-means algorithm on the original dataset to find i clusters, and sum the distance of all
points from their cluster mean. Call this sum the dispersion.

Generate a set of reference datasets (of the same size as the original). One simple way of
generating a reference dataset is to sample uniformly from the original dataset’s bounding 
rectangle; a more sophisticated approach is take into account the original dataset’s shape
by sampling, say, from a rectangle formed from the original dataset’s principal components.

Calculate the dispersion of each of these reference datasets, and take their mean.

Define the ith gap by: log(mean dispersion of reference datasets) - log(dispersion of original dataset).

Once we’ve calculated all the gaps (we can add confidence intervals as well;
see the original paper for the formula), we can select the number of clusters to be
the one that gives the maximum gap. (Sidenote: I view the gap statistic as a very
statistical-minded algorithm, since it compares the original dataset against a set of
reference “control” datasets.)


// Read in the original dataset

// Generate a set of Referene datasets save them all to file.

// for a range of ks,
// Run kmeans on the original dataset using the current k
// Calculate the dispersion for the original dataset
// Run kmeans for each of the reference datasets...Calculate dispersion for each of them
// Calculate the mean of the dispersion of all the datasets
// Calculate the gap using the formula

// Look into the condidence intervals

// Select the cluster with the max gap

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

int findK(const std::string& mpi_args,const std::string& kmeans_dir, const std::string& other, int& bestK)
{
    // Create a std::map from the user id to the ratingMatrix
    std::vector<std::vector<double> > ratingMatrix;

    std::cout<<"Getting the RatingMatrix...\n";
    int errorVal =  getRatingMatrix("ratings_with_id.txt", ratingMatrix);
    std::cout<<"RatingMatrix Dimensions: "<<ratingMatrix.size()<<", "<<ratingMatrix[0].size()<<endl;
    int numberOfUsers = ratingMatrix.size();

    // TODO: Generate a set of reference datsets

     // Find a way to get the upper limit
    int upperLimit = sqrt(numberOfUsers);

    // For arange of K values
    std::map<int, double> gapStatistic;
    for(int k = 2; k < upperLimit; k++)
    {

    }
}