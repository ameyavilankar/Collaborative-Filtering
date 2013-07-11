#ifndef PREPROCESS_H
#define PREPROCESS_H

//preprocess.h v1.0 
// Author: Ameya Vilankar

#include <vector>
#include <map>
#include <iostream>
#include <math.h>

const double H = 2;
const int NUM_FEATURES = 100;

std::vector<std::vector<double> > getCosineMatrix(const std::vector<std::vector<double> >& ratingMatrix, std::map<long, int> userMap, std::vector<long> userVector);

inline void convertToDistances(std::vector<std::vector<double> >& cosineDistances)
{
    // Convert from the similarity to distances
    for(int i = 0; i < cosineDistances.size(); i++)
	for(int j = 0; j < cosineDistances[i].size(); j++)
	    cosineDistances[i][j] = exp(-cosineDistances[i][j]/H);	
}

inline void normalizeAlongColumns(std::vector<std::vector<double> >& cosineDistances)
{
    // Find the magnitude along each column
    std::vector<double> magnitude(cosineDistances[0].size());
    for(int j = 0; j < cosineDistances[0].size(); j++)
    {	
	for(int i = 0; i < cosineDistances.size(); i++)
	    magnitude[j] += cosineDistances[i][j] * cosineDistances[i][j];
	    
	magnitude[j] = sqrt(magnitude[j]);
    }

    // Normalise l2 along the columns
    for(int i = 0; i < cosineDistances.size(); i++)
	for(int j = 0; j < cosineDistances[i].size(); j++)
	    cosineDistances[i][j] /= magnitude[j];
}

inline void normalizeAlongRows(std::vector<std::vector<double> >& cosineDistances)
{
    // Find the magnitude along each row
    std::vector<double> magnitude(cosineDistances.size());
    
    for(int i = 0; i < cosineDistances.size(); i++)
    {	
	for(int j = 0; j < cosineDistances[i].size(); j++)	
	    magnitude[i] += cosineDistances[i][j] * cosineDistances[i][j];
	    
	magnitude[i] = sqrt(magnitude[i]);
    }

    // Normalise l2 along the rows
    for(int i = 0; i < cosineDistances.size(); i++)
	for(int j = 0; j < cosineDistances[i].size(); j++)
	    cosineDistances[i][j] /= magnitude[i];
}

void normalizeMatrix(std::vector<std::vector<double> >& cosineDistances);
void preprocessMatrix(std::vector<std::vector<double> >& cosineDistances);

#endif
