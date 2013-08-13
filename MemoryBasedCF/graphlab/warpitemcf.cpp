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


/**
 * \graphitembasedcf.cpp
 *
 * \brief This file contains a GraphLab based implementation of the
 * Item Based Collaboarive Filtering.
 *
 * \author Ameya Vilankar, Pipefish LLC <ameya.vilankar@gmail.com>.
 * Co-authors: Danny Bickson <danny.bickson@gmail.com>
 			   Yucheng Low <ylow@cs.cmu.edu>
 */

#include <vector>
#include <algorithm>
#include <utility>
#include <queue>
#include <boost/math/special_functions/gamma.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
//#include <graphlab/parallel/atomic.hpp>

#include <graphlab.hpp>
#include <graphlab/warp.hpp>
#include <graphlab/util/stl_util.hpp>
#include "stats.hpp"


enum DISTANCE_METRICS {
	JACKARD = 0,
	AA = 1,
	RA = 2,
	PEARSON = 3,
	COSINE = 4,
	CHEBYCHEV = 5,
	MANHATTEN = 6,
	TANIMOTO = 7,
	LOG_LIKELIHOOD = 8,
	JACCARD_WEIGHT = 9
};

/*
* \brief Add 2 to negative node id to prevent -0 and -1 which are not allowed as vertex ids.
*/
const int SAFE_NEG_OFFSET = 2; 

/*
* \brief The id of the user/item on the vertex.
*/
typedef int id_type;

/*
* \brief Type to store the rating on the edge of the graph and in other sparse vectors
*/
typedef double rating_type;

/*
* \brief Type for the list of items/user to be saved on an vertex.
*/
typedef std::map<id_type, rating_type> rated_type;

/*
* \brief Type for a sparse Matrix
*/
typedef std::map<id_type, rated_type> sparse_matrix;

/**
* \brief We use the rated_items type in accumulators and so we define an
* operator+= so that we can combine two rated_items into a single one
*/
inline rated_type& operator+=(rated_type& left, const rated_type& right)
{
	// Add entries from right if right is not empty
	if(!right.empty())
	{
		if(left.empty())
			left = right;
		else
		{
			// TODO: Check the += operator
			// Add to left it exits else += will add to 0 which is same =
			for(rated_type::const_iterator cit = right.begin(); cit != right.end(); cit++)
				left[cit->first] += cit->second; // left[it->first]++;
		}
	}

	return left;
}

inline void combine(rated_type& left, const rated_type& right)
{
	// Add entries from right if right is not empty
	if(!right.empty())
	{
		if(left.empty())
			left = right;
		else
		{
			// TODO: Check the += operator
			// Will only add new entry if not present
			for(rated_type::const_iterator cit = right.begin(); cit != right.end(); cit++)
				if(left.find(cit->first) == left.end())
					left[cit->first] = cit->second;
		}
	}
}

inline sparse_matrix& operator+=(sparse_matrix& left, const sparse_matrix& right)
{
	if(!right.empty())
	{
		if(left.empty())
			left = right;
		else
		{
			// TODO: Check the += operator
			// Add to left it exits else += will add to 0 which is same =
			for(sparse_matrix::const_iterator cit = right.begin(); cit != right.end(); cit++)
				left[cit->first] = cit->second; // left[it->first]++;
		}
	}

	return left;
}


/**
* \brief This is used to find the intersection (common items/users) from two lists of users/items
*
*/
inline rated_type intersect(const rated_type& left, const rated_type& right)
{
	rated_type intersection;

	// Find the users common in both left and right maps and add them to intersection
	for(rated_type::const_iterator cit = left.begin(); cit != left.end(); cit++)
		if(right.find(cit->first) != right.end())
			intersection[cit->first] = 1;

	return intersection;
}

/**
 * \ The number of top similar items to be used in the calculation
 */
size_t TOPK = 10;

/**
 * \brief The number of top similar items to be used in the calculation
 */
size_t MIN_ALLOWED_INTERSECTION = 1;

/**
 * \brief Global map to hold the average rating for each user.
 */
rated_type user_average;

/**
 * \brief Global sparse matrix to hold the ratings for each item.
 */
sparse_matrix item_vector;


/**
 * \brief The similarity value to be returned when there are less than MIN_ALLOWED_INTERSECTION
 * common elements between two sparse vectors.
 */
const int INVALID_SIMILARITY = -2;

/**
* \brief Used to calculate the adjusted cosine similarity between two sparse vectors
*/
double adj_cosine_similarity(rated_type& one, rated_type& two)
{
	rated_type oneCommon, twoCommon;

	for(rated_type::const_iterator one_it = one.begin(); one_it != one.end(); one_it++)
	{
		// if a movie is rated by both one and two
		if(two.find(one_it->first) != two.end())
		{
			oneCommon[one_it->first] = one_it->second;
			twoCommon[one_it->first] = two[one_it->first];
		}
	}

	if(oneCommon.size() < MIN_ALLOWED_INTERSECTION)
		return INVALID_SIMILARITY;
	
	double oneSqrSum = 0.0;
	double twoSqrSum = 0.0;
	double prodSum = 0.0;
	
	for(rated_type::const_iterator one_it = oneCommon.begin(); one_it != oneCommon.end(); one_it++)
	{
		prodSum += (one_it->second * twoCommon[one_it->first]);
		oneSqrSum += one_it->second * one_it->second;
		twoSqrSum += twoCommon[one_it->first] * twoCommon[one_it->first];
  	}
	
	double denominator = sqrt(oneSqrSum * twoSqrSum);
	
	if(denominator == 0)
		return 0;
	else
		return prodSum/denominator;
}

/**
 * \brief The vertex data represents each item and user in the
 * dataset and contains the rating given by each user to each movie/item.
 */
struct vertex_data
{
	// Total number of times the vertex has been updated...incremented everytime apply is run
	uint32_t num_updates;

	// The average rating of item/user..
	rating_type average_rating;

	// The list of rated items for a user vertex or
	// The list of users who have given a rating, for an item vertex
	rated_type rated_items;

	// Used to store the recommended items: Filled ony for user vertices
	rated_type recommended_items;

	// Constructor
	vertex_data(rating_type avg = 0.0): num_updates(0.0), average_rating(avg), rated_items() {}

	// Functions to make vertex serializable
	void save(graphlab::oarchive& arc) const
	{
		arc << num_updates << average_rating << rated_items << recommended_items;
	}

	void load(graphlab::iarchive& arc)
	{
		arc >> num_updates >> average_rating >> rated_items >> recommended_items;
	}

}; // End of the vertex class


/*
* \brief The edge class connects an user to an item he as rated.
*  The class holds the rating given by the user to an item.
*/
struct edge_data
{
	// The rating for an edge
	rating_type rating;

	// Constructor
	edge_data(rating_type rat = 0.0): rating(rat) {}

	// Functions to make edge_data serializable
	void save(graphlab::oarchive& arc) const
	{
		arc << rating;
	}

	void load(graphlab::iarchive& arc)
	{
		arc >> rating;
	}

}; // End of the edge_data class


/**
 * \brief The Item Based graph is a bipartite graph with Items connected to
 * Users if the User has rated the Item.
 *
 * The edges store the rating given by the User to the Item. All the edges are 
 * directed from the user to the Items.
 *
 */
typedef graphlab::distributed_graph<vertex_data, edge_data> graph_type;

/*
* \brief Defines the engine that is used to run programs over the vertices 
* of the distributed graph.
*/
typedef graphlab::warp::warp_engine<graph_type> engine_type;

/**
 * \brief The graph loader is used by graph.load() to parse lines of the
 * text data file. This function is parallelized and loads one line at a time.
 *
 * We use the relativley fast boost::spirit parser to parse each line.
 */
bool graph_loader(graph_type& graph, const std::string& fname, const std::string& line)
{
	// Check for empty lines
	ASSERT_FALSE(line.empty());

	// Shorten Namespaces
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	namespace phoenix = boost::phoenix;

	// Used to store the vertex ids for items and users
	graphlab::vertex_id_type user_id(-1), item_id(-1);

	// Used to store the rating on the edge
	rating_type rating = 0;

	// Read using the boost libraries
	const bool success = qi::phrase_parse
	(line.begin(), line.end(),       
		//  Begin grammar
		(
		qi::ulong_[phoenix::ref(user_id) = qi::_1] >> -qi::char_(',') >>
		qi::ulong_[phoenix::ref(item_id) = qi::_1] >> -qi::char_(',') >>
		qi::ulong_[phoenix::ref(rating) = qi::_1]
		)
	,
	//  End grammar
	ascii::space);

	// Quit if the reading failed
	if(!success)
		return false;  

	// Since this is a bipartite graph I need a method to number the
	// user and item/movie vertices differently.  To accomplish I make sure
	// all vertices have non-zero ids and then negate the user vertex.
	// Unfortunatley graphlab reserves -1 and so we add 2 and negate.
	// Resulting user_id after adding 2, cannot be 1 because negating it would result 
	// in -1 which is reserved
	user_id += SAFE_NEG_OFFSET;
	ASSERT_GT(user_id, 1);
	user_id = -user_id;
	ASSERT_NE(user_id, item_id);

	// Create an edge and add it to the graph
	graph.add_edge(user_id, item_id, edge_data(rating));

	// successful load
	return true;
} // end of graph_loader 


/**
 * \brief Determine if the given vertex is a item vertex or a   
 * vertex.
 *
 * For simplicity we connect users --> items and therefore if a vertex
 * has in edges then it is a item.
 */
inline bool is_item(const graph_type::vertex_type& vertex)
{
	return vertex.num_in_edges() > 0 ? true : false;
}


/**
 * \brief Determine if the given vertex is a user vertex
 *
 * For simplicity we connect users --> items and therefore if a vertex
 * has out edges then it is a user
 */
inline bool is_user(const graph_type::vertex_type& vertex)
{
	return vertex.num_out_edges() > 0 ? true : false;
}


/**
 * \brief Get the other vertex in the edge.
 */
inline graph_type::vertex_type get_other_vertex(const graph_type::edge_type& edge, const graph_type::vertex_type& vertex)
{
	return vertex.id() == edge.source().id()? edge.target() : edge.source();
}


/*
* \brief The gather type holds the rating contributed by each edge
* to a users average rating. It also holds the list of rated of items rated by a user.
*/
struct gather_type
{
	// To hold the average rating(or the sum of ratings) contributed by each edge
	rating_type rating;

	// This will be used to store the list of users/items
	rated_type items;

	// Constructor: Used to create using rating and an empty vector
	gather_type(rating_type rat = 0.0, rated_type ri = rated_type()): rating(rat), items() {}

	// Constructor: Used to create using rating and rated_items map containing only one id
	gather_type(rating_type rat, id_type id): rating(rat)
	{
		items[id] = rating;
	}

	// Functions to make gather_type serialisable
	void save(graphlab::oarchive& arc) const
	{
		arc << rating << items;
	}

	void load(graphlab::iarchive& arc)
	{
		arc >> rating >> items;
	}

	// Used to sum up in the gather step
	gather_type& operator+=(const gather_type& right)
	{
		this->rating += right.rating;
		this->items += right.items;

		return *this; 
	}
};

/*
* \brief 
* For User Vertices:
* The gather_type structure that is made up of the rating on the edge and 
* the rated_items map that contains the item connected to the user (other vertex) and its rating.
* 
* For the Item Vertices:
* The gather_type structure that is made up of the rating on the edge and 
* the rated_items map that contains the user connected to the item (other vertex) and its rating.
*/
gather_type map_get_sparse_vectors(graph_type::edge_type edge, graph_type::vertex_type other)
{
	//graphlab::vertex_id_type other_id = (other.id() > 0)? other.id(): -(other.id() + SAFE_NEG_OFFSET);
	return gather_type(edge.data().rating, other.id());
}

/*
* \brief
* For User Vertices:
* Calculates the average of all the ratings for a user and a sparse vector
* containing the item_ids and their rating.
* 
* For the Item Vertices:
* Calculates the average of all the ratings for an item and a sparse vector
* containing the user_ids and their rating.
*
*/
void get_sparse_vectors(engine_type::context& context, graph_type::vertex_type vertex)
{
	// Get the number of rated items/users by the users/items
	// Users: in_edges will be zero and Items: out_edges will be zero
	const size_t num_rated = vertex.num_in_edges() + vertex.num_out_edges();
	ASSERT_GT(num_rated, 0);

	// Gather on all the edges to get the average rating and the sparse vector containing the items/users and their ratings
	gather_type gather_result = graphlab::warp::map_reduce_neighborhood<gather_type, graph_type::vertex_type>(vertex, graphlab::ALL_EDGES, map_get_sparse_vectors);

	// Get a reference to the vertex data
	vertex_data& vdata = vertex.data();

	// Calculate and set the average user/item rating for the vertex
	vdata.average_rating = gather_result.rating/num_rated;

	// Save the sparse of the rated items/users on the current vertex
	vdata.rated_items = gather_result.items;

	/*
	// Set the rating as 1 if a user for counting
	if(is_user(vertex))
	{
		for(rated_type::iterator it = vdata.rated_items.begin(); it != vdata.rated_items.end(); it++)
			it->second = 1.0;
	}
	*/

	// Increment the num_updates to the vertex by 1
	vdata.num_updates++;
}

/*
sparse_matrix map_get_sparse_matrix(graph_type::edge_type edge, graph_type::vertex_type other)
{
	sparse_matrix ret_mat;
	ret_mat[other.id()] = other.data().rated_items;
	return ret_mat;
}
*/
	

/*
* \brief Returns the sparse vector containing all the items rated by the user vertex.
*/
rated_type map_get_similar_items(graph_type::edge_type edge, graph_type::vertex_type other)
{
	// return the list of items rated by the user on the other side of the edge
	return other.data().rated_items;
}

/*
* \brief 
* Aggregates in parallel the list of items rated by each user by map reducing on all the user
* vertices connected to an item. It also counts the number of users common between the current item
* and the items in the aggregated list. It then removes the item that have less than MIN_ALLOWED_INTERSECTION
* common users. 
*/
void get_similar_items(engine_type::context& context, graph_type::vertex_type vertex)
{
	// Gather the list of items rated by each user.
	rated_type gather_result = graphlab::warp::map_reduce_neighborhood<rated_type, graph_type::vertex_type>(vertex, graphlab::IN_EDGES, map_get_similar_items, combine);

	// Remove the current item from the list
	gather_result.erase(vertex.id());	

	// Get a reference to the vertex data
	vertex_data& vdata = vertex.data();

	// Store the list of similar items into the recommended items
	vdata.recommended_items = gather_result;

	// Increment the num_updates to the vertex by 1
	vdata.num_updates++;
}

struct item_similarity
{
	id_type id;
	double similarity;

	item_similarity(id_type i, double sim = 0.0):id(i), similarity(sim) {}
};

struct compare
{
	bool operator()(const item_similarity& one, const item_similarity& two)
	{
		return one.similarity > two.similarity;
	}
};


void calculate_topk(engine_type::context& context, graph_type::vertex_type vertex)
{
	// Get a reference to the list of similar items to compare to
	rated_type& items = vertex.data().recommended_items;

	// Current id to be used in distance computation
	id_type current_id = vertex.id();

	// Calculate the similarity score of current user to all the other user
	rated_type::iterator it = items.begin();
	while(it != items.end())
	{
		// Get the score and check if it is valid
		double score = adj_cosine_similarity(item_vector[it->first], item_vector[current_id]);
		if(score == INVALID_SIMILARITY)
		{
			// Score is invalid if common users are less than MIN_ALLOWED_INTERSECTION
			// If score is invalid remove it from the list
			items.erase(it++);
		}
		else
		{
			// Score is valid, set the score
			it->second = score; 
			it++;
		}	
	}

	// Trim the result to have only topk entries using a heap
	std::priority_queue<item_similarity, std::vector<item_similarity>, compare> min_queue;
	
	for(rated_type::const_iterator cit = items.begin(); cit != items.end(); cit++)
		if(min_queue.size() == TOPK)
		{
			if(cit->second > min_queue.top().similarity)
			{
				min_queue.pop();
				min_queue.push(item_similarity(cit->first, cit->second));
			}
		}
		else
			min_queue.push(item_similarity(cit->first, cit->second));

	// Used to store the top-k similar items
	rated_type reco_items;
	item_similarity temp(0, 0);
	while (!min_queue.empty())
	{
		// Get the element at the top and save it to top-k similar items
		temp = min_queue.top();
		reco_items[temp.id] = temp.similarity;

		// Remove the element
		min_queue.pop();
	}

	// Save the new top-k items to the vertex
	items = reco_items;
	
	// Increment the num_updates to the vertex by 1
	vertex.data().num_updates++;
}

/*
* \brief Used to compute the average of each user
*/
struct user_average_reducer
{
	rated_type user_average;

	static user_average_reducer get_user_average(const graph_type::vertex_type& v)
	{
		// Just create a map that maps from the user_id to its average rating for the current vertex
	    user_average_reducer result;
	    result.user_average[v.id()] = v.data().average_rating;
	    return result;
  	}

  	user_average_reducer& operator+=(const user_average_reducer& other)
  	{
  		// add all the entries from other to the current one
  		for(rated_type::const_iterator cit = other.user_average.begin(); cit != other.user_average.end(); cit++)
  			user_average[cit->first] = cit->second;

	    return *this;
  	}

  	// Functions to make gather_type serialisable
	void save(graphlab::oarchive& arc) const
	{
		arc << user_average;
	}

	void load(graphlab::iarchive& arc)
	{
		arc >> user_average;
	}

};

/*
* \brief Used to compute the vector for each item and build up a 
* sparse matrix contatining all item vectors.
*/
struct item_vector_reducer
{
	sparse_matrix item_vector;

	static item_vector_reducer get_item_vector(const graph_type::vertex_type& v)
	{
		// Just create a map that maps from the item_id to its vector
	    item_vector_reducer result;
	    result.item_vector[v.id()] = v.data().rated_items;
	    return result;
  	}

  	item_vector_reducer& operator+=(const item_vector_reducer& other)
  	{
  		// add all the entries from other to the current one
  		for(sparse_matrix::const_iterator cit = other.item_vector.begin(); cit != other.item_vector.end(); cit++)
  			item_vector[cit->first] = cit->second;

	    return *this;
  	}

  	// Functions to make gather_type serialisable
	void save(graphlab::oarchive& arc) const
	{
		arc << item_vector;
	}

	void load(graphlab::iarchive& arc)
	{
		arc >> item_vector;
	}

};

struct topk_reducer
{
	sparse_matrix topk;

	static topk_reducer get_topk(const graph_type::vertex_type& v)
	{
		// Just create a map that maps from the item_id to its vector
	    topk_reducer result;
	    result.topk[v.id()] = v.data().recommended_items;
	    return result;
  	}

  	topk_reducer& operator+=(const topk_reducer& other)
  	{
  		// add all the entries from other to the current one
  		for(sparse_matrix::const_iterator cit = other.topk.begin(); cit != other.topk.end(); cit++)
  			topk[cit->first] = cit->second;

	    return *this;
  	}

  	// Functions to make gather_type serialisable
	void save(graphlab::oarchive& arc) const
	{
		arc << topk;
	}

	void load(graphlab::iarchive& arc)
	{
		arc >> topk;
	}

};

/*
* \brief Used to Hold the following:
* 1. Weighted Sum: This holds the summation of the rating for an item 
* multiplied by its similarity score.
* 2. This holds the summation of the similarity scores for normalising
*/
struct recommended_items
{
	rated_type weighted_sum;
	rated_type similarity_sum;
	
	// Constructor
	recommended_items():weighted_sum(), similarity_sum() {}

	// Functions to make gather_type serialisable
	void save(graphlab::oarchive& arc) const
	{
		arc << weighted_sum << similarity_sum;
	}

	void load(graphlab::iarchive& arc)
	{
		arc >> weighted_sum >> similarity_sum;
	}

	// Implement the operator+= for aggregating together
	recommended_items& operator+=(const recommended_items& right)
	{
		// the operator+= is already defined for rated_type
		this->weighted_sum += right.weighted_sum;
		this->similarity_sum += right.similarity_sum;

		return *this; 
	}
};

/*
* \brief 
*/
// TODO: Make it more efficient..see if map can be copied(copy constructor)
recommended_items map_get_recommended_items(graph_type::edge_type edge, graph_type::vertex_type other)
{
	// Get the rating for the edge which is used to caclulate the weighted sum
	rating_type rating = edge.data().rating;

	// Get a reference to the similar items on the connected item vertex
	const rated_type& items = other.data().recommended_items;
	
	// The result to returned 
	recommended_items result;

	// Get the weighted sum and the similarity scores
	for(rated_type::const_iterator cit = items.begin(); cit != items.end(); ++cit)
	{
		result.weighted_sum[cit->first] = cit->second * rating;
		result.similarity_sum[cit->first] = cit->second;
	}
	
	return result; 
}

/*
* \brief 
*/
void get_recommended_items(engine_type::context& context, graph_type::vertex_type vertex)
{
	// Gather on all the edges to get the average rating and the sparse vector containing the items/users and their ratings
	recommended_items result = graphlab::warp::map_reduce_neighborhood<recommended_items, graph_type::vertex_type>(vertex, graphlab::ALL_EDGES, map_get_recommended_items);

	// Get a reference to the vertex data
	vertex_data& vdata = vertex.data();

	// Add only those movies that are not rated by the user
	for(rated_type::const_iterator cit = result.weighted_sum.begin(); cit != result.weighted_sum.end(); ++cit)
		if(vdata.rated_items.find(cit->first) == vdata.rated_items.end())
			vdata.recommended_items[cit->first] = (cit->second/result.similarity_sum[cit->first]);

	// Increment the num_updates to the vertex by 1
	vdata.num_updates++;
}

/*
* \brief Used to save the results to file
*/
class graph_writer
{
public:

	// TODO save in JSON Format
	std::string save_vertex(graph_type::vertex_type v)
	{
		if(is_user(v))
		{
			// Stringstream is slower...replace with boost spirit
			std::stringstream strm;
			strm << "User id: " << -(v.id() + SAFE_NEG_OFFSET) << "\n";

			strm << "Rated items: ";
			for(rated_type::const_iterator cit = v.data().rated_items.begin(); cit != v.data().rated_items.end(); cit++)
				strm << "(" << cit->first << ", " << cit->second << ")";
			strm << "\n";
			
			strm << "Recommended Items: ";
			for(rated_type::const_iterator cit = v.data().recommended_items.begin(); cit != v.data().recommended_items.end(); ++cit)
				strm << "(" << cit->first << ", " << cit->second << ")"; 
			strm << "\n\n\n";

			return strm.str();
		}
		else
		{
			// Stringstream is slower...replace with boost spirit
			std::stringstream strm;
			strm << "Item id: " << v.id() << "\n";

			strm << "Users who rated item: ";
			for(rated_type::const_iterator cit = v.data().rated_items.begin(); cit != v.data().rated_items.end(); cit++)
				strm << "(" << cit->first << ", " << cit->second << ")";
			strm << "\n";
			
			strm << "Top K Similar Items: ";
			for(rated_type::const_iterator cit = v.data().recommended_items.begin(); cit != v.data().recommended_items.end(); ++cit)
				strm << "(" << cit->first << ", " << cit->second << ")"; 
			strm << "\n\n\n";

			return strm.str();	
		}
	}

	// No need to save edge data since it only contatins rating
	std::string save_edge(graph_type::edge_type e)
	{ 
		return ""; 
	}
};

int main(int argc, char** argv)
{
	// Parse command line options -----------------------------------------------
	const std::string description = "Carry out Item Based Collaborative Filtering.";
	graphlab::command_line_options clopts(description);
	std::string input_file = "inputfile.txt";
	std::string predictions;
	std::string exec_type = "synchronous";
	int min_allowed_intersection = 1;
	int distance_metric = COSINE;

	// TODO: Add more commandlines
	clopts.attach_option("input_file", input_file,
						"Input file name in sparse matrix market format");
	clopts.attach_option("predictions", predictions,
	                   "The prefix (folder and filename) to save predictions.");
	clopts.attach_option("engine", exec_type, 
	                   "The engine type synchronous or asynchronous");
	clopts.attach_option("min_allowed_intersection", min_allowed_intersection,
						"The minimum number of common users that have rated two items for considering for similarity computation.");
	clopts.attach_option("distance", distance_metric, 
						"The type of distance to be used in the item-item similarity computation.");
	clopts.attach_option("topk", TOPK,
						"The number of similar items that should be used in prediction step.");

	// Check if input file was specified
	if(input_file.empty())
	{
		std::cout << "Input file is: " << input_file << "\n";
		std::cout << "No Input File specified. Please Specify a file in the sparse matrix format." << std::endl;
		clopts.print_description();
		return EXIT_FAILURE;
	}

	// Check if a valid distance metric was specified
	if (distance_metric != PEARSON && distance_metric != MANHATTEN && distance_metric != COSINE &&
		distance_metric != CHEBYCHEV && distance_metric != LOG_LIKELIHOOD && distance_metric != TANIMOTO && distance_metric != JACKARD)
	{
		std::cout << "Error in parsing distance distance metric." << std::endl;
		clopts.print_description();
		return EXIT_FAILURE;	
	}


	if(!clopts.parse(argc, argv) || input_file == "")
	{
		std::cout << "Error in parsing command line arguments." << std::endl;
		clopts.print_description();
		return EXIT_FAILURE;
	}
	
	// Initialize MPI
    graphlab::mpi_tools::init(argc, argv);
    // Construct distributed control object
    graphlab::distributed_control dc;

    
    dc.cout() << "Loading Graph...\n";
    graphlab::timer timer;
    // Create the distributed graph object
    timer.start();
    graph_type graph(dc, clopts);
    // Load the graph in parallel on multiple machines using the parser function
    graph.load(input_file, graph_loader);
    dc.cout() << "Loading Graph Finished in " << timer.current_time() << "\n";


    dc.cout() << "Finalizing Graph...\n";
    timer.start();
    // Commit the graph and distribute it across machines
    graph.finalize();
    dc.cout() << "Finalizing Graph finished in " << timer.current_time() << "\n";


  	dc.cout() 
		<< "========== Graph statistics on proc " << dc.procid() 
		<< " ==============="
		<< "\n Num vertices: " << graph.num_vertices()
		<< "\n Num edges: " << graph.num_edges()
		<< "\n Num replica: " << graph.num_replicas()
		<< "\n Replica to vertex ratio: " 
		<< float(graph.num_replicas())/graph.num_vertices()
		<< "\n --------------------------------------------" 
		<< "\n Num local own vertices: " << graph.num_local_own_vertices()
		<< "\n Num local vertices: " << graph.num_local_vertices()
		<< "\n Replica to own ratio: " 
		<< (float)graph.num_local_vertices()/graph.num_local_own_vertices()
		<< "\n Num local edges: " << graph.num_local_edges()
		<< "\n Edge balance ratio: " 
		<< float(graph.num_local_edges())/graph.num_edges()
		<< std::endl;


    dc.cout() << "Getting the User Vertex set...\n";
    graphlab::vertex_set user_set = graph.select(is_user);
    size_t num_users = graph.vertex_set_size(user_set);
    dc.cout() << "Number of Users in Graph: " << num_users << "\n";

    dc.cout() << "Getting the Item Vertex set...\n";
    graphlab::vertex_set item_set = graph.select(is_item);
    size_t num_items = graph.vertex_set_size(item_set);
    dc.cout() << "Number of Items in Graph: " << num_items << "\n"; 
    
    
    dc.cout() << "Calculating User average and items rated by user...\n";
    dc.cout() << "Calculating Item average and users who rated the items...\n";
    timer.start();
	// Creates the warp engine
	engine_type engine(dc, graph);
	// Sets the update function to use
	engine.set_update_function(get_sparse_vectors);
	// Run the update functions on all the vertices to store their sparse vectors on them
	engine.signal_all();
	// Run the engine until scheduler is empty.
	engine.start();    
	dc.cout() << "Finished in " << timer.current_time() << "\n";

	// Run map_reduce on all the user vertices to get the global average user vectors
	dc.cout() << "Calculating the average rating for each user...\n";
	timer.start();
    user_average = graph.map_reduce_vertices<user_average_reducer>(user_average_reducer::get_user_average, user_set).user_average;

	// Run map_reduce on all the item_vertices to get the global sparse matrix of item vectors
    dc.cout() << "Getting the vector for each item using map reduce on item vertices...\n";
    item_vector = graph.map_reduce_vertices<item_vector_reducer>(item_vector_reducer::get_item_vector, item_set).item_vector;
	dc.cout() << "Finished in " << timer.current_time() << "\n";

	/*
	// Debug code
    dc.cout() << "Displaying the average rating for each user.\n";
    for(rated_type::const_iterator cit = user_average.begin(); cit != user_average.end(); cit++)
    	dc.cout() << "User: " << cit->first << ", Average Rating: " << cit->second << "\n";

    dc.cout() << "Displaying the Global Item Matrix.\n";
    for(sparse_matrix::const_iterator cit = item_vector.begin(); cit != item_vector.end(); cit++)
    {
    	dc.cout() << "Item: " << cit->first << "\n" << "Item vectors:\n";
		for(rated_type::const_iterator uit = cit->second.begin(); uit != cit->second.end(); uit++)
    		dc.cout() << "User: " << uit->first << ", Rating: " << uit->second << "\n";    	
    }
    */

	// Get the list of similar items and the
	dc.cout() << "Calculate the List of similar items to compare to for each item...\n";
	timer.start();
	engine.set_update_function(get_similar_items);
	engine.signal_vset(item_set);
	engine.start();
	dc.cout() << "Finished in " << timer.current_time() << "\n\n";

	// Get the list of similar items and the
	dc.cout() << "Getting the top K similar items out of all the similar items...\n";
	timer.start();
	engine.set_update_function(calculate_topk);
	engine.signal_vset(item_set);
	engine.start();
	dc.cout() << "Finished in " << timer.current_time() << "\n\n";

	/*	
	// Debug Code	
    dc.cout() << "Getting the TOP SIMILAR ITEMS for each item using map reduce on item vertices...\n";
    sparse_matrix topk = graph.map_reduce_vertices<topk_reducer>(topk_reducer::get_topk, item_set).topk;
	dc.cout() << "Finished in " << timer.current_time() << "\n";

	dc.cout() << "Displaying the Top k itmes.\n";
    for(sparse_matrix::const_iterator cit = topk.begin(); cit != topk.end(); cit++)
    {
    	dc.cout() << "Item: " << cit->first << "\n" << "Similar Items:\n";
		for(rated_type::const_iterator uit = cit->second.begin(); uit != cit->second.end(); uit++)
    		dc.cout() << "Item: " << uit->first << ", Score: " << uit->second << "\n";    	
    }
	*/

	// Calculate the Recommendations for each of the users
	dc.cout() << "Calculating the Recommendations for each of the User: \n";
	timer.start();
	engine.set_update_function(get_recommended_items);
	engine.signal_vset(user_set);
	engine.start();
	dc.cout() << "Finished in " << timer.current_time() << "\n\n";

    // Save the predictions if indicated by the user
    /*
    if(!predictions.empty())
    {
    	*/
	    std::cout << "Saving the Recommended Items for each User...\n";
	    const bool gzip_output = false;
	    const bool save_vertices = true;
	    const bool save_edges = false;
	    const size_t threads_per_machine = 2;

	    //save the predictions
	    graph.save(predictions, graph_writer(),
	               gzip_output, save_vertices, 
	               save_edges, threads_per_machine);

	//}

	// Close MPI and return success
	graphlab::mpi_tools::finalize();
	return EXIT_SUCCESS;

}// end of main