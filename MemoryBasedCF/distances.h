#ifndef DISTANCES_H
#define DISTANCES_H

// distances.h v1.1
// Author: Ameya Vilankar

#include <vector>
#include <string>
#include <iostream>
#include <math.h>
#include <map>
#include <limits>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/*
// Function pointer that contains which distance function to call
double (*distance_pointer)(const std::vector<double>& one, const std::vector<double>& two);

double findCommon(const std::vector<double>& one, const std::vector<double>& two, distance_pointer distance)
{
  assert(one.size() == two.size());

  std::vector<double> oneCommon;
  std::vector<double> twoCommon;

  for(int i = 0; i < one.size()l i++)
    if(one[i] != 0 || two[i] != 0)
    {
      oneCommon.push_back(one[i]);
      twoCommon.push_back(two[i]);
    }

  return distance(oneCommon, twoCommon);
}
*/

// used to calculate the euclidean distance between two vectors
double euclidean(const std::vector<double>& one, const std::vector<double>& two);

// used to calculate the mean of the vector
double mean(const std::vector<double> one);

// calculates the pearson coefficient between two vectors
double pearsonCoefficient(const std::vector<double>& one, const std::vector<double>& two);

// Finds the common rated entries and calls pearson coefficient
double calcPearson(const std::vector<double>& one, const std::vector<double>& two);

// Finds the common rated entries and calls euclidean distance
double calcEuclidean(const std::vector<double>& one, const std::vector<double>& two);

// Calculate the similarity measure
double calcEuclideanSimilarity(const std::vector<double>& one, const std::vector<double>& two);

// calculates the cosine similarity between two vectors
double consineDistance(const std::vector<double>& one, const std::vector<double>& two);

// calculates the jaccardDistance between the two vectors
double jaccardDistance(const std::vector<double>& one, const std::vector<double>& two);

// calculates the tanimoto distance between two vectors 
double tanimoto(const std::vector<double>& one, const std::vector<double>& two);

// calculates the mahattan distance between two vectors
double manhattanDistance(const std::vector<double>& one, const std::vector<double>& two);

// calculates the chebychev distance between the two vertices
double chebychev(const std::vector<double>& one, const std::vector<double>& two);

// NOT SURE ABOUT THIS DISTANCE MEASURE
double slopeOneDistance(const std::vector<double>& one, const std::vector<double>& two);

#endif
