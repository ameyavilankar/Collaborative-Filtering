#ifndef CONSTANTS_H
#define CONSTANTS_H

// constants.h - v1.0
// Author: Ameya Vilankar

// used in the calculation of the cosine distance
const double H = 1;

// Number of features to reduced to in the dimensionality reduction part
const int NUM_FEATURES = 100;

// Total number of eigenvalues in the SVD step i.e the dimension of the eigenvalue matrix
const int NUM_EIGEN_VALUES = 100;

// Number of singular values to consider (k-rank approximation SVD)
const int K_RANK = 10;

// TODO: Add all parameters for SVD and KMEANS if doing it through code and not the shell script

// Number of Clusters in kmeans
const int NUM_OF_CLUSTERS = 55;


#endif