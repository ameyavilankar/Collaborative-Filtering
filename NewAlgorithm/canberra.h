#ifndef CANBERRA_H
#define CANBERRA_H

#include <string>
#include <cmath>
#include <cstdio>
//#include <unordered_map>
#include <map>
#include <algorithm>  
#include <iostream>
#include <vector>

// Finds the harmonic value for the value n
double harm(long n)
{
	double		h = 0.0;

	for (long i = 1; i <= n; i++)
		h += 1.0 / (double)i;

	return (h);
}

double e_harm(long n)
{
	return (0.5 * harm(floor((double)n / 2.0)));
}

double o_harm(long n)
{
	return (harm(n) - 0.5 * harm(floor((double)n / 2.0)));
}

double a_harm(long n)
{
	return (n % 2 ? o_harm(n) : e_harm(n));
}

double exact_canberra(long ne, long k)
{
	double		sum =  0.0;

	for (long t = 1; t <= k; t++)
		sum += t * (a_harm(2 * k - t) - a_harm(t));

	return (2.0 / ne * sum + (2.0 * (ne - k) / ne) * (2 * (k + 1) * (harm(2 * k + 1) - harm(k + 1)) - k));
}

double xi(long s)
{
	return ((s + 0.5) * (s + 0.5) * harm(2 * s + 1) - 0.125 * harm(s) - 0.25 * (2.0 * s * s + s + 1.0));
}

double eps(long k, long s)
{
	return (0.5 * (s - k) * (s + k + 1.0) * harm(s + k + 1) + 0.5 * k * (k + 1) * harm(k + 1) + 0.25 * s * (2.0 * k - s - 1.0));
}

double delta(long a, long b, long c)
{
	double d = 0.0;

	for (long i = a; i <= b; i++)
		d += (double)fabs(c - i) / (double)(c + i);

	return (d);
}


// Declarations of the methods
double canberra_location(long nl, long ne, std::vector<std::vector<long> >& lists, long k, std::vector<long>& i1, std::vector<long>& i2, std::vector<double>& dist);
double average_partial_list(long nl, long ne, std::vector<std::vector<long> >& lists);
double pnormalizer(long ne, long nm);
double canberra_quotient(long nl, long ne, std::vector<std::vector<long> >& lists, bool complete, bool normalize, std::vector<long>& i1, std::vector<long>& i2, std::vector<double>& dist);
double canberra(std::vector< std::vector<long> >& rlists, int rk, bool rdist);
double max_dist(int p);
double canberraq(std::vector< std::vector<long> >& rlists, bool rcomplete, bool rnormalize, bool rforcenormal, bool rdist);
double maxdist(int p);
double calculate_canberradist(const std::vector<double>& array_one, const std::vector<double>& array_two);

// Datastructure to hold the movie and the rating together
struct MovieRating
{
	long movie_id;
	double rating;

	// Constructor
	MovieRating(long id = 0, double r = -1):movie_id(id), rating(r) {}
};

// Used to compare to movies according to their rating
bool compare(const MovieRating& a, const MovieRating& b)
{
	return (a.rating > b.rating);
}

#endif
