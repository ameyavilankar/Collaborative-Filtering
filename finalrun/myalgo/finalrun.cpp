/*
 * Copyright (c) 2009 Carnegie Mellon University.
 *     All rights reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS
 *  IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 *  express or implied.  See the License for the specific language
 *  governing permissions and limitations under the License.
 *
 * For more about this software visit:
 *
 *      http://www.graphlab.ml.cmu.edu
 *
 */

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <graphlab.hpp>
#include "finalrun.h"
 
//remove assigned options from arguments
std::string get_arg_str_without(int argc, char** argv, std::vector<std::string> remove_opts)
{
    std::stringstream strm;
    bool skip_next = false;
    
    for (int i = 1; i < argc; ++i)
    {
        bool skip = false;
        for (size_t j = 0; j < remove_opts.size(); ++j)
        {
            std::string with_equal = remove_opts[j] + "=";
            if (strncmp(with_equal.c_str(), argv[i], with_equal.size()) == 0)
            {
                skip = true;
            }
            else
                if (strncmp(remove_opts[j].c_str(), argv[i], remove_opts[j].size()) == 0)
                {
                    skip = true;
                    skip_next = true;
                }
        }
    
        if (skip == false && skip_next == false)
        {
            strm << argv[i] << " ";
        }
        else
            if (skip == false && skip_next == true)
            {
                skip_next = false;
            }
    }

    return strm.str();
}

bool call_graph_laplacian(const std::string& mpi_args, const std::string& filename, const std::string& format,
    const bool normalized_cut, const bool ratio_cut, const std::string& args)
{
    std::stringstream strm;
    
    if(mpi_args.length() > 0)
        strm << "mpiexec " << mpi_args << " ";
    strm << "./graph_laplacian ";
    strm << " --graph=" << filename;
    strm << " --format=" << format;
    //  strm << " --normalized-cut=" << normalized_cut;
    //  strm << " --ratio-cut=" << ratio_cut;
    strm << " " << args;
    std::cout << "CALLING >" << strm.str() << std::endl;
    int sys_ret = system(strm.str().c_str());
    
    if (sys_ret != 0)
    {
        std::cout << "system call fails" << std::endl;
        return false;
    }

    return true;
}

bool call_svd(const std::string& mpi_args, const std::string& filename,
    const std::string& svd_dir, const size_t num_clusters, const size_t rank,
    const size_t rows, const size_t cols, const std::string& args)
{
    // To hold the entire function call
    std::stringstream strm;
    
    // Add only if running distributed using MPI
    if(mpi_args.length() > 0)
        strm << "mpiexec " << mpi_args << " ";
    
    // Build the function call
    strm << svd_dir << "svd " + filename /*+ ".glap"*/;
    strm << " --rows=" << rows;
    strm << " --cols=" << cols;
    strm << " --nsv=" << rank;
    strm << " --nv=" << rank;
    strm << " --max_iter=" << (rank - 1);
    strm << " --quiet=1";
    strm << " --save_vectors=1";
    strm << " --ortho_repeats=3";
    strm << " --id=1";
    strm << " --prediction=" << filename + ".";
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

bool call_eigen_vector_normalization(const std::string& mpi_args,
    const std::string& filename, const size_t num_clusters, const size_t rank,
    const size_t num_data, const std::string& args)
{
    std::stringstream strm;
    
    if(mpi_args.length() > 0)
        strm << "mpiexec " << mpi_args << " ";
    strm << "./eigen_vector_normalization";
    strm << " --data=" << filename;
    strm << " --clusters=" << num_clusters;
    strm << " --rank=" << rank;
    strm << " --data-num=" << num_data;
    strm << " " << args;
    std::cout << "CALLING >" << strm.str() << std::endl;
    int sys_ret = system(strm.str().c_str());
    
    if (sys_ret != 0)
    {
        std::cout << "system call fails" << std::endl;
        return false;
    }

    return true;
}


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

//select good rank
int get_lanczos_rank(const size_t num_clusters, const size_t num_data)
{
    size_t rank = 1;
    
    if (num_data < 1000)
    {
        if (num_clusters + 10 <= num_data)
            rank = num_clusters + 10;
        else
            rank = num_data;
    }
    else
        if (num_data < 10000)
            rank = num_clusters + 25;
        else
            if (num_data < 100000)
                rank = num_clusters + 50;
            else
                if (num_data < 1000000)
                    rank = num_clusters + 80;
                else
                    rank = num_clusters + 100;

    return rank;
}


// Get the number of lines from the file to get the number of users
int countLines(const std::string& filename = "Output")
{
    // Load the singular Values and create the singular matrix
    ifstream infile;
    infile.open(filename.c_str());

    //Always test the file open.
    if(!infile) 
    {
        std::cout<<"Error opening output file"<<std::endl;
        return -1;
    }

    // new lines will be skipped unless we stop it from happening:    
    infile.unsetf(std::ios_base::skipws);

    // count the newlines with an algorithm specialized for counting:
    unsigned line_count = std::count(std::istream_iterator<char>(infile), std::istream_iterator<char>(), '\n');

    std::cout << "Lines: " << line_count << "\n";

    return line_count;
}

// Get the number of lines from the file to get the number of users
int countDimension(const std::string& filename = "Output")
{
    // Load the singular Values and create the singular matrix
    std::ifstream infile;
    infile.open(filename.c_str());

    //Always test the file open.
    if(!infile) 
    {
        std::cout<<"Error opening output file"<<std::endl;
        return -1;
    }

    std::string firstline;
    std::getline(infile, firstline);

    // Split the firstline and only return the double parts
    std::vector<double> splitDouble = split(firstline);
    std::cout << "Lines: " << splitDouble.size() - 1 << "\n";

    return splitDouble.size() - 1;
}


int main(int argc, char** argv)
{
    std::cout << "Graph partitioning using Dimensionality Reduction and Clustering...\n\n";

    std::string graph_dir = "";
    std::string format = "adj";
    std::string svd_dir = "../collaborative_filtering/";
    std::string kmeans_dir = "../clustering/";
    std::string mpi_args;
    std::string kmeansinput = "kmeansinput.txt";
    size_t num_partitions = 2;

    //parse command line
    graphlab::command_line_options clopts("Graph partitioning (normalized cut)");
    clopts.attach_option("graph", graph_dir, "The graph file. This is not optional. Vertex ids must start from 1 "
    "and must not skip any numbers.");
    clopts.attach_option("format", format, "The graph file format. If \"weight\" is set, the program will read "
    "the data file where each line holds [id1] [id2] [weight].");
    clopts.attach_option("partitions", num_partitions, "The number of partitions to create");
    clopts.attach_option("svd-dir", svd_dir, "Path to the directory of Graphlab svd");
    clopts.attach_option("kmeans-dir", kmeans_dir, "Path to the directory of Graphlab kmeans");
    clopts.attach_option("mpi-args", mpi_args, "If set, will execute mipexec with the given arguments. "
    "For example, --mpi-args=\"-n [N machines] --hostfile [host file]\"");
    

    if (!clopts.parse(argc, argv))
        return EXIT_FAILURE;
    
    cout << "Calculating the Cosine Matrix...\n";
    // Build the cosine Matrix and save it to file name "Output"
    int errorVal = generate_cosine_matrix();
    if(errorVal != 0)
        return EXIT_FAILURE;

    // Count the number of lines(users)
    size_t num_data = countLines(graph_dir + "ratings_with_id.txt");
    
    // Count the number of columns(movies)
    size_t num_cols = countDimension(graph_dir + "ratings_with_id.txt");
    
    //determine the rank of Lanczos method
    size_t rank = get_lanczos_rank(num_partitions, num_data);
    
    // detemine the number of partitions
    num_partitions = (int)(sqrt(num_data))/2;
 
   
    std::vector<std::string> remove_opts;
    remove_opts.push_back("--graph");
    remove_opts.push_back("--format");
    remove_opts.push_back("--svd-dir");
    remove_opts.push_back("--kmeans-dir");
    remove_opts.push_back("--partitions");
    remove_opts.push_back("--mpi-args");
    std::string other_args = get_arg_str_without(argc, argv, remove_opts);

     
    // Run SVD on the matrix stored in Output
    if (call_svd(mpi_args, graph_dir + "Output", svd_dir, num_partitions, rank, num_data, num_cols, other_args) == false)
        return EXIT_FAILURE;

    // Use the SVD Output to Calculate the k-rank approximation and store it in kmeansinput.txt
    std::cout << "Calculating the K-Rank Approximation...\n"; 
    errorVal = calculate_kmeans_input(rank);
    if(errorVal != 0)
        return EXIT_FAILURE;

    // run kmeans
    std::cout << "Running Kmeans on the output of SVD..\n";
    if (call_kmeans(mpi_args, kmeansinput, kmeans_dir, num_partitions, other_args) == false)
        return EXIT_FAILURE;
    
    // make kmeans input and other variables global in constants.h
    std::cout << "Doing Calculations on the Output of the Kmeans...\n";
    errorVal = visualise_cosine();
    if(errorVal != 0)
	return EXIT_FAILURE;
    
    return EXIT_SUCCESS;
}

