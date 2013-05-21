#include "canberra.h"

// Using Declarations
using std::floor;
using std::fabs;
using std::vector;
using std::cout;
using std::endl;
using std::map;


// TODO Check for reference passing or const reference passing
double canberra_location(long nl, long ne, std::vector<std::vector<long> >& lists, long k, std::vector<long>& i1, std::vector<long>& i2, std::vector<double>& dist)
{
	long		i, idx1, idx2, l1, l2, count;
	double		distance, indicator;

	indicator = 0.0;
	count = 0;

	for (idx1 = 1; idx1 <= nl - 1; idx1++)
		for (idx2 = idx1 + 1; idx2 <= nl; idx2++) 
		{
			distance = 0.0;
			for (i = 1; i <= ne; i++) 
			{
				l1 = lists[idx1 - 1][i - 1] + 1 <= k + 1 ? lists[idx1 - 1][i - 1] + 1 : k + 1;
				l2 = lists[idx2 - 1][i - 1] + 1 <= k + 1 ? lists[idx2 - 1][i - 1] + 1 : k + 1;
				distance += fabs(l1 - l2) / (l1 + l2);
			}

			i1[count] = idx1 - 1;
			i2[count] = idx2 - 1;
			dist[count] = distance;
			count++;

			indicator += 2.0 * distance / (nl * (nl - 1));
		}

	return (indicator);
}

double average_partial_list(long nl, long ne, std::vector<std::vector<long> >& lists)
{
	long		i, j;
	double		nm = 0.0;
	double		tmp;

	for (i = 0; i < nl; i++) {
		tmp = 0.0;
		for (j = 0; j < ne; j++)
			if (lists[i][j] > -1)
				tmp++;
		nm += tmp / nl;
	}

	return (nm);
}

double pnormalizer(long ne, long nm)
{
	return ((1.0 - exact_canberra(nm, nm) / exact_canberra(ne, ne)));
}

double canberra_quotient(long nl, long ne, std::vector<std::vector<long> >& lists, bool complete, bool normalize, std::vector<long>& i1, std::vector<long>& i2, std::vector<double>& dist)
{
	long		i, idx1, idx2, count;
	long		t1, t2, ii;
	long		p, l1, l2, l1tmp, l2tmp, j;
	double		distance, indicator, tmp2, tmp3;
	long           *intersection;
	std::vector<long>  list1, list2;
	long		common;
	long		unused;
	double		A;
	double		nm;


	p = ne;
	indicator = 0.0;
	count = 0;

	for (idx1 = 1; idx1 <= nl - 1; idx1++) {

		l1tmp = 0;
		for (i = 1; i <= ne; i++)
			if (lists[(idx1 - 1)][(i - 1)] > -1)
				l1tmp++;

		for (idx2 = idx1 + 1; idx2 <= nl; idx2++) {
			l2tmp = 0;
			for (i = 1; i <= ne; i++)
				if (lists[(idx2 - 1)][i - 1] > -1)
					l2tmp++;

			if (l1tmp <= l2tmp) {
				list1 = lists[idx1 - 1];
				list2 = lists[idx2 - 1];
				l1 = l1tmp;
				l2 = l2tmp;
			} else {
				list2 = lists[idx1 - 1];
				list1 = lists[idx2 - 1];
				l1 = l2tmp;
				l2 = l1tmp;
			}

			common = 0;
			for (i = 1; i <= ne; i++)
				if (list1[i - 1] > -1 && list2[i - 1] > -1)
					common++;

			intersection = (long *)malloc(common * sizeof(long));

			unused = 0;
			j = 0;
			for (i = 1; i <= ne; i++) {
				if (list1[i - 1] > -1 && list2[i - 1] > -1)
					intersection[j++] = i;
				if (list1[i - 1] == -1 && list2[i - 1] == -1)
					unused++;
			}

			distance = 0.0;
			tmp2 = 0.0;
			tmp3 = 0.0;
			for (i = 0; i <= common - 1; i++) {
				ii = intersection[i];
				t1 = list1[ii - 1] + 1;
				t2 = list2[ii - 1] + 1;
				distance += fabs(t1 - t2) / (t1 + t2);
				tmp2 += delta(l2 + 1, p, t1);
				tmp3 += delta(l1 + 1, p, t2);
			}


			if (p != l2)
				distance += 1.0 / (p - l2) *
					(-tmp2 + l1 * (p - l2) - 2.0 * eps(p, l1) + 2.0 * eps(l2, l1));

			if (p != l1)
				distance += 1.0 / (p - l1) *
					(-tmp3 + (p - l1) * l1 - 2.0 * eps(p, l1) + 2.0 * eps(l1, l1) +
					 2.0 * (xi(l2) - xi(l1)) -
					 2.0 * (eps(l1, l2) - eps(l1, l1) + eps(p, l2) - eps(p, l1)) +
					 (p + l1) * (l2 - l1) + l1 * (l1 + 1.0) - l2 * (l2 + 1.0));

			if (p != l1 && p != l2 && complete == true) {
				A = (1.0 * unused) / ((p - l1) * (p - l2));

				distance += A * (2.0 * xi(p) - 2.0 * xi(l2)
				      - 2.0 * eps(l1, p) + 2.0 * eps(l1, l2)
				      - 2.0 * eps(p, p) + 2.0 * eps(p, l2) +
						 (p + l1) * (p - l2) + l2 * (l2 + 1.0) - p * (p + 1.0));
			}

			i1[count] = idx1 - 1;
			i2[count] = idx2 - 1;
			dist[count] = distance;
			count++;

			indicator += 2.0 * distance / (nl * (nl - 1));

			free(intersection);
		}
	}

	if (normalize == true) {
		nm = average_partial_list(nl, ne, lists);
		indicator /= pnormalizer(ne, nm);
	}

	return (indicator);
}

double canberra(std::vector< std::vector<long> >& rlists, int rk, bool rdist)
{
	if (rlists.empty())
		return NULL;
	
	std::vector< std::vector<long> > lists = rlists;
	int		k = rk;
	int		i;
	long		ne;
	long		nl;
	
	
	nl = rlists.size();

	if (nl == 0)
		return NULL;
	
	// TODO: Check
	//if (lists == NULL)
	//	return NULL;

	ne = lists[0].size();

	for (i = 0; i < nl; i++)
		if (ne != lists[i].size())
			return NULL;
	
	if ((k > ne) || k <= 0) {
		return NULL;
	}

	float		size = (nl * (nl - 1)) / 2;

	std::vector<long>		i1_v(size);
	std::vector<long>		i2_v(size);
	std::vector<double>		dist_v(size);

	double		distance = canberra_location(nl, ne, lists, k, i1_v, i2_v, dist_v);
	double		exact = exact_canberra(ne, k);

	double		distnorm = distance / exact;

	return distnorm;
	
}

/* from R6 in http://mpba.fbk.eu/files/publications/jurman09canberra.pdf */
double max_dist(int p)
{

	div_t q = div(p,4);
	/* printf("q.quot is %d, q.rem is %d\n", q.quot, q.rem); */
	
	double r = q.quot;
	double maxdist=0;
	switch(q.rem) {
		case 0:
			maxdist = (2.0*r*(harm(3.0*r) - harm(r)));
		break;


		case 1:
			maxdist = ((2.0*r + 1.0)*harm(6.0*r + 1.0) + r*harm(3.0*r + 1.0) - (r + 0.5)*harm(3.0*r) - (2.0*r + 1.0)*harm(2.0*r +1.0) + 0.5*harm(r));
		break;


		case 2:
			maxdist = ((2.0*r +1.0)*(2.0*harm(6.0*r + 3.0) - harm(3.0*r + 1.0) - 2.0*harm(2.0*r + 1.0) + harm(r)));
		break;


		case 3:
			maxdist = ((2.0*r + 1.0)*harm(6.0*r + 5.0) + 0.5*harm(3.0*r + 2.0) - (2.0*r + 1.0)*harm(2.0*r + 1.0) - (r + 1.0)*harm(r+1.0) + (r + 0.5)*harm(r));
		break;

/*
		default:
			printf("Hit the default case! Not mathematically possible!\n"); 
*/

	}
	
	/* printf("maxdist is %lf\n",maxdist); */
	return(maxdist);
}


double canberraq(std::vector< std::vector<long> >& rlists, bool rcomplete, bool rnormalize, bool rforcenormal, bool rdist)
{
	std::vector< std::vector<long> > lists = rlists;
	int		i, nl, ne;

	if (lists.empty())
		return 0;
	
	nl = rlists.size();
	ne = rlists[0].size();

	/* printf("nl is %d, ne is %d\n",nl, ne); */
	for (i = 0; i < nl; i++) {
		if (ne != lists[i].size()) {
			return NULL;
		}
	}

	int		size = (nl * (nl - 1)) / 2;
	double maxdist;

	std::vector<long>		i1_v(size);
	std::vector<long>		i2_v(size);
	std::vector<double>		dist_v(size);

	double		distance = canberra_quotient(nl, ne, lists, rcomplete, rnormalize, i1_v, i2_v, dist_v);
	double		exact = exact_canberra(ne, ne); /* we don't support top-k lists */
	double distnorm;
	
	if(rforcenormal == true) {
		maxdist = max_dist(ne);
		//printf("distance is %lf, maxdist is %lf,exact is %lf\n",distance,maxdist,exact);
		distnorm = distance / max_dist(ne);
	} else {
		distnorm = distance / exact;
	}

	return distnorm;
}



double maxdist(int p)
{
	return max_dist(p);
}		

/*
  #
  # Calculate the canberra distance between two arrays
  #  Note: this canberra implementation provides an array prefilter
  #  to ensure that arrays of disparate elements and sizes can be
  #  processed
  #
*/  

// New Function: To be called from K-means
double calculate_canberradist(const vector<double>& array_one, const vector<double>& array_two)
{
	if(array_one.empty() || array_two.empty())
		return 1;
	else
	{
		// used to sort the movies for each user accoriding to the ratings
		vector<MovieRating> user_one;
		vector<MovieRating> user_two;

		// Universe of all movies commonly rated by the users
		//std::unordered_map<long, vector<long> > movie_universe;
		map<long, int> movie_universe;
		int movieCount = 0;


		cout<<endl<<endl;

		// Either user has rated the movie
		for(int i = 0; i < array_one.size(); i++)
		{
			// Either user has rated the movie
			if(array_one[i] != 0 || array_two[i] != 0)
			{
				cout<<"array_one "<<i<<" :"<<array_one[i]<<endl;
				cout<<"array_two "<<i<<" :"<<array_two[i]<<endl;

				user_one.push_back(MovieRating(i, array_one[i]));
				
				user_two.push_back(MovieRating(i, array_two[i]));

				//add the movie to the movie universe
				movie_universe[i] = movieCount;
				movieCount++;
			}
		}

		cout<<endl<<endl;
		cout<<"Size of the user_one: "<<user_one.size()<<endl;
		cout<<"Size of the user_two: "<<user_two.size()<<endl;

		cout<<endl<<endl;
		cout<<"Movie Ids in user_one and their rating: "<<endl;
		for (int i = 0; i < user_one.size(); i++)
		{
			cout<<user_one[i].movie_id<<" "<<array_one[user_one[i].movie_id]<<endl;
		}	

		cout<<endl;
		cout<<"Movie Ids in user_two and their rating: "<<endl;
		for (int i = 0; i < user_two.size(); i++)
		{
			cout<<user_two[i].movie_id<<" "<<array_two[user_two[i].movie_id]<<endl;
		}

		
		// Stable sort the two MovieRating Pair
		stable_sort(user_one.begin(), user_one.end(), compare);
		stable_sort(user_two.begin(), user_two.end(), compare);

		// Final matrix to be returned, and the two columns of the matrix
		vector<vector<long> > rlists;
		vector<long> rank_one(user_one.size());
		vector<long> rank_two(user_two.size());
		
		for(int i = 0; i < user_one.size(); i++)
		{
			if(user_one[i].rating != 0)
				rank_one[movie_universe[user_one[i].movie_id]] = i;
			else
				rank_one[movie_universe[user_one[i].movie_id]] = -1;

			if(user_two[i].rating != 0)
				rank_two[movie_universe[user_two[i].movie_id]] = i;
			else
				rank_two[movie_universe[user_two[i].movie_id]] = -1;
		}

		cout<<endl<<endl;
		cout<<"Final Ranks for user_one: ";
		for (int i = 0; i < rank_one.size(); i++)
			cout<<rank_one[i]<<" ";
		cout<<endl;

		cout<<"Final Ranks for user_two: ";
		for (int i = 0; i < rank_two.size(); i++)
			cout<<rank_two[i]<<" ";
		cout<<endl;

		rlists.push_back(rank_one);
		rlists.push_back(rank_two);

		return canberraq(rlists, false, false, true, false);
	}
		
}


int main(int argc, char const *argv[])
{
	std::vector<double> rating_user_one(10);
	std::vector<double> rating_user_two(10);

	rating_user_one[0] = 1.0;
	rating_user_one[1] = 3.0;
	rating_user_one[3] = 1.0;
	rating_user_one[5] = 5.0;
	rating_user_one[7] = 2.0;

	
	cout<<"Rating One:";
	for(int i = 0; i < rating_user_one.size(); i++)
		cout<<rating_user_one[i]<<" ";
	cout<<endl;

	rating_user_two[1] = 1.0;
	rating_user_two[4] = 3.0;
	rating_user_two[5] = 1.0;
	rating_user_two[7] = 4.0;
	rating_user_two[9] = 2.0;

	cout<<"Rating two:";
	for(int i = 0; i < rating_user_two.size(); i++)
		cout<<rating_user_two[i]<<" ";
	cout<<endl<<endl;

	// Set the two rating matrices
	double distance = calculate_canberradist(rating_user_one, rating_user_two);
	cout<<"Distance: "<<distance<<endl;

	return 0;
}
