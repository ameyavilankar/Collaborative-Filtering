#ifndef KMEANS_INPUT_H
#define KMEANS_INPUT_H

// kmeansinput.h - v1.0 
// Author: Ameya Vilankar

#include "constants.h"
#include "readMatrix.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <Eigen/Dense>

// Calculates the input matrix to the kmeans step using the SVD Output Matrices and singualr values
int calculate_kmeans_input(size_t rank);

#endif
