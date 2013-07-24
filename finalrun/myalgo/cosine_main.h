#ifndef COSINE_H
#define COSINE_H

// cosine.h - v1.0
// Author: Ameya Vilankar

// TODO: About the .cpp files
#include "math.h"

#include "preprocess.h"
#include "preprocess.cpp"

#include "readMatrix.h"
#include "readMatrix.cpp"

#include "distances.h"
#include "constants.h"

#include "to_string.h"

// Genrates the dense low dimension cosine_matrix
int generate_cosine_matrix();

#endif