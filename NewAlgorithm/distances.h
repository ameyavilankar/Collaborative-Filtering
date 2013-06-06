#include <vector>
#include <string>
#include <iostream>
#include <math.h>
#include <map>
#include <limits>
#include <assert.h>
#include <stdio.h>

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

double pearsonCoefficient(const std::vector<double>&, const std::vector<double>&);
double euclidean(const std::vector<double>&, const std::vector<double>&);

double calcPearson(const std::vector<double>& one, const std::vector<double>& two)
{
	std::vector<double> oneCommon;
	std::vector<double> twoCommon;
	
	for(int i = 0; i < one.size(); i++)
		if(one[i] != 0 && two[i] != 0)
		{
			oneCommon.push_back(one[i]);
			twoCommon.push_back(two[i]);
		}
	
	//std::cout<<"oneCommon: "<<oneCommon.size()<<" , TwoCommon: "<<twoCommon.size()<<std::endl;
	
	return pearsonCoefficient(oneCommon, twoCommon);
}

double calcEuclidean(const std::vector<double>& one, const std::vector<double>& two)
{
	std::vector<double> oneCommon;
	std::vector<double> twoCommon;
	
	for(int i = 0; i < one.size(); i++)
		if(one[i] != 0 && two[i] != 0)
		{
			oneCommon.push_back(one[i]);
			twoCommon.push_back(two[i]);
		}
	
	//std::cout<<"oneCommon: "<<oneCommon.size()<<" , TwoCommon: "<<twoCommon.size()<<std::endl;
	
	return euclidean(oneCommon, twoCommon);
}


// used to calculate the euclidean distance between two vectors
double euclidean(const std::vector<double>& one, const std::vector<double>& two)
{
  assert(one.size() == two.size());

  double sum = 0.0;
  for(int i = 0; i < one.size(); i++)
    sum += (one[i] - two[i]) * (one[i] - two[i]);

  return sum;
}

// used to calculate the mean of the vector
double mean(const std::vector<double> one)
{
  assert(one.size() >= 0);

  double mean = 0.0;
  for(int i = 0 ; i < one.size(); i++)
    mean += one[i];

  return mean/one.size();
}

// calculates the pearson coefficient between two vectors
double pearsonCoefficient(const std::vector<double>& one, const std::vector<double>& two)
{
  assert(one.size() == two.size());

  // Calculate the mean of the two vectors
  double meanOne = mean(one);
  double meanTwo = mean(two);

  // Calculate the square of difference and the product of the differences
  double oneSqrDiff = 0.0;
  double twoSqrDiff = 0.0;
  double prodDiff = 0.0;
  double oneDiff = 0.0;
  double twoDiff = 0.0;

  for(int i = 0; i < one.size(); i++)
  {
    oneDiff = (one[i] - meanOne);
    twoDiff = (two[i] - meanTwo);

    prodDiff += oneDiff * twoDiff;
    oneSqrDiff += oneDiff * oneDiff;
    twoSqrDiff += twoDiff * twoDiff;
  }
  
  double denominator = sqrt(oneSqrDiff * twoSqrDiff);
	
  /*
  std::cout<<"Pearson Coefficient: "<<prodDiff/denominator<<std::endl;
  std::cout<<"oneSqrDiff: "<<oneSqrDiff<<" twoSqrDIff "<<twoSqrDiff<<std::endl;
  std::cout<<"OneCommon: ";
  
  for(int i = 0; i < one.size(); i++)
	  std::cout<<one[i]<<" ";
  std::cout<<std::endl;
  
  for(int i = 0; i < two.size(); i++)
	  std::cout<<two[i]<<" ";
  std::cout<<std::endl;
  */

  if(denominator == 0)
	  return 0;

  return prodDiff/denominator;
}

// calculates the cosine similarity between two vectors
double consineDistance(const std::vector<double>& one, const std::vector<double>& two)
{
  assert(one.size() == two.size());

  double oneSqrSum = 0.0;
  double twoSqrSum = 0.0;
  double prodSum = 0.0;
  for(int i = 0; i < one.size(); i++)
  {
    prodSum += (one[i] * two[i]);
    oneSqrSum += one[i] * one[i];
    twoSqrSum += two[i] * two[i];
  }

  double denominator = sqrt(oneSqrSum * twoSqrSum);
  
  if(denominator == 0)
    return 1.0;
  else
    return 1.0 - prodSum/denominator;
}

// calculates the jaccardDistance between the two vectors
double jaccardDistance(const std::vector<double>& one, const std::vector<double>& two)
{
  assert(one.size() == two.size());

  int unionCount = 0;
  int intersectionCount = 0;

  for(int i = 0 ; i < one.size(); i++)
  {
    if(one[i] != 0 && two[i] != 0)
      intersectionCount++;

    if(one[i] != 0 || two[i] != 0)
      unionCount++;
  }

  if(unionCount == 0);
    return 1.0;

  return 1.0 - (double)intersectionCount/unionCount;
}

// calculates the tanimoto distance between two vectors 
double tanimoto(const std::vector<double>& one, const std::vector<double>& two)
{
  assert(one.size() == two.size());

  double oneSqrSum = 0.0;
  double twoSqrSum = 0.0;
  double prodSum = 0.0;
  
  for(int i = 0; i < one.size(); i++)
  {
    prodSum += (one[i] * two[i]);
    oneSqrSum += one[i] * one[i];
    twoSqrSum += two[i] * two[i];
  }

  if((oneSqrSum + twoSqrSum - prodSum) == 0)
    return 1.0;

  return prodSum/(oneSqrSum + twoSqrSum - prodSum);
}

double manhattanDistance(const std::vector<double>& one, const std::vector<double>& two)
{
  assert(one.size() == two.size());

  double distance = 0.0;
  for(int i = 0; i < one.size(); i++)
    distance += abs(one[i] - two[i]);

  return distance;
}

double chebychev(const std::vector<double>& one, const std::vector<double>& two)
{
  assert(one.size() == two.size());

  double max = std::numeric_limits<double>::min();

  for(int i = 0; i < one.size(); i++)
    if(abs(one[i] - two[i]) > max)
      max = abs(one[i] - two[i]);

  return max;
}

// NOT SURE ABOUT THIS DISTANCE MEASURE
double slopeOneDistance(const std::vector<double>& one, const std::vector<double>& two)
{
  assert(one.size() == two.size());

  double sumDiff = 0.0;
  for(int i = 0; i < one.size(); i++)
  {
    sumDiff += (one[i] - two[i]);
  }

  return sumDiff;
}
