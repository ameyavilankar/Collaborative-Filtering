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
#include <boost/math/special_functions/gamma.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
//#include <graphlab/parallel/atomic.hpp>

#include <graphlab/warp.hpp>
#include <graphlab.hpp>
#include <graphlab/util/stl_util.hpp>
#include "stats.hpp"

/*
enum DISTANCE_METRICS{
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
*/


/*
* \brief Add 2 to negative node id to prevent -0 and -1 which are not allowed as vertex ids.
*/
const int SAFE_NEG_OFFSET = 2; 

/*
* \brief The id of the user/item on the vertex.
*/
typedef int id_type;

/*
* \brief Type to store the rating on the edge of the graph(can be changed to int for 0-5 range)
*/
typedef double rating_type;

/*
* \brief Type for the list of items/user to be saved on an vertex.
*/
typedef std::map<id_type, rating_type> rated_items_type;

/**
* \brief We use the rated_items type in accumulators and so we define an
* operator+= so that we can combine two rated_items into a single one
*/
inline rated_items_type& operator+=(rated_items_type& left, const rated_items_type& right)
{
	// Handle Addition to self
	if(left == right)
	{
		rated_items_type newRight;

		for(rated_items_type::const_iterator it = right.begin(); it != right.end(); it++)
				newRight[it->first] = it->second;

		// call += again, but will not go into this block of code
		left += newRight;

		return left;
	}

	// Add entries from right if right is not empty
	if(!right.empty())
	{
		if(left.empty())
			left = right;
		else
		{
			for(rated_items_type::const_iterator it = right.begin(); it != right.end(); it++)
				left[it->first]++;
		}
	}

	return left;
}

/**
* \brief This is used to find the intersection (common items/users) from two lists of users/items
*
*/
inline rated_items_type intersect(const rated_items_type& left, const rated_items_type& right)
{
	rated_items_type intersection;

	// Find the users common in both left and right maps and add them to intersection
	for(rated_items_type::const_iterator cit = left.begin(); cit != left.end(); cit++)
		if(right.find(cit->first) != right.end())
			intersection[cit->first] = 1;

	return intersection;
}

/**
 * \brief The number of users in the graph.
 */
size_t NUM_USERS = 1;

/**
 * \brief The number of items/movies in the graph.
 */
size_t NUM_ITEMS = 1;


/**
 * \brief The number of top similar items to be used in the calculation
 */
size_t TOPK = 10;

/**
 * \brief The number of top similar items to be used in the calculation
 */
size_t MIN_ALLOWED_INTERSECTION = 10;

/**
* \brief Used to calculate the cosine similarity between two sparse vectors
*/
double cosineSimilarity(const rated_items_type& one, const rated_items_type& two)
{
	rated_items_type oneCommon, twoCommon;

	for(rated_items_type::const_iterator one_it = one.begin(); one_it != one.end(); one_it++)
	{
		// if a movie is rated by both one and two
		if(two.find(one_it->first) != two.end())
			two[one_it->first] = 0;
	}

	assert(oneCommon.size() == twoCommon.size());

	if(oneCommon.size() < MIN_ALLOWED_INTERSECTION)
		return -1;
	
	double oneSqrSum = 0.0;
	double twoSqrSum = 0.0;
	double prodSum = 0.0;
	
	for(map<long, double>::const_iterator one_it = oneCommon.begin(); one_it != oneCommon.end(); one_it++)
	{
		prodSum += (one_it->second * twoCommon[one_it->first]);
		oneSqrSum += one_it->second * one_it->second;
		twoSqrSum += twoCommon[one_it->first] * twoCommon[one_it->first];
  	}
	
	double denominator = sqrt(oneSqrSum * twoSqrSum);
	
	if(denominator == 0)
		return 0;0
	else
		return prodSum/denominator;
}

/**
 * \brief The vertex data represents each item and user in the
 * dataset and contains the rating given by each user to each movie/item.
 */
struct vertex_data
{
	// Stores the id on the movie/item
	id_type data_id;

	// Total number of times the vertex has been updated...incremented everytime apply is run
	uint32_t num_updates;

	// The average rating of item/user..
	rating_type average_rating;

	// The list of rated items for a user vertex or
	// The list of users who have given a rating for an item vertex
	rated_items_type rated_items;

	// Used to store the recommended items: Filled ony for user vertices
	rated_items_type recommended_items;

	// Constructor
	vertex_data(int id = 0, rating_type avg = 0.0): data_id(id), num_updates(0.0), average_rating(avg), rated_items() {}

	// Functions to make vertex serializable
	void save(graphlab::oarchive& arc) const
	{
		arc << data_id << num_updates << average_rating << rated_items << recommended_items;
	}

	void load(graphlab::iarchive& arc)
	{
		arc >> data_id >> num_updates >> average_rating >> rated_items >> recommended_items;
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
typedef graphlab::warp_engine<graph_type> engine_type;

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
	double rating = 0;

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
} // end of graph 


/**
 * \brief Determine if the given vertex is a item/movie vertex or a   
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
	rated_items_type items;

	// Constructor: Used to create using rating and an empty vector
	gather_type(rating_type rat = 0.0, rated_items_type ri = rated_items_type()): rating(rat), items() {}

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
gather_type map_get_sparse_vectors(graph_type::edge_type& edge, const graph_type::vertex_type& other)
{
	return gather_type(edge.data().rating, other.data().data_id);
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
// TODO : confirm whether there should be & for vertex?
void get_sparse_vectors(engine_type::context& context, graph_type::vertex_type vertex)
{
	// Get the number of rated items/users by the users/items
	const size_t num_rated = vertex.num_in_edges() + vertex.num_in_edges();
	ASSERT_GT(num_rated, 0);

	// Gather on all the edges to get the average rating and the sparse vector containing the items/users and their ratings
	gather_type gather_result = warp::map_reduce_neighborhood(vertex, graphlab::ALL_EDGES, map_get_rated_items);

	// Get a reference to the vertex data
	vertex_data& vdata = vertex.data();

	// Calculate and set the average user/item rating for the vertex
	vdata.average_rating = gather_result.rating/num_rated_items;

	// Save the sparse of the rated items/users on the current vertex
	vdata.rated_items = gather_result.items;

	if(is_user(vdata))
	{
		for(rated_items_type::iterator it = vdata.rated_items.begin(); it != vdata.rated_items.end(); it++)
			it->second = 1.0;
	}

	// Increment the num_updates to the vertex by 1
	vdata.num_updates++;
}

rated_items_type map_get_topk(graph_type::edge_type& edge, const graph_type::vertex_type& other)
{
	// return the list of items rated by the user
	return other.data().rated_items;
}

rated_items_type combine(rated_items_type& one, rated_items_type& two)
{

}

void get_topk(engine_type::context& context, graph_type::vertex_type vertex)
{
	rated_items_type top_k = map_reduce_neighborhood(vertex, graphlab::IN_EDGES, map_get_topk);

	// Remove those users who have less than MIN_ALLOWED_INTERESECTION COMMON
	rated_items_type::iterator it = top_k.begin(); 
	while (it != top_k.end())
	{
		if (it->first < MIN_ALLOWED_INTERSECTION)
			top_k.erase(it++);
		else
			it++;
	}

	// Calculate similarity
}

/*
* \brief 
* Graph Writer used to store all the vertex data(The recommended items and their predictions) to file for a user vertex.
* We don't store anything for an item vertex.
* We don't store the edge data because it is just the rating.
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
			strm << v.data().data_id << "\t";
			
			for(rated_items_type::const_iterator cit = v.data().recommended_items.begin(); cit != v.data().recommended_items.end(); ++cit)
				strm << "(" << cit->first << ", " << cit->second << ")"; 

			return strm.str();
		}

		// TODO Check
		return "\n";
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
	std::string input_dir;
	std::string predictions;
	std::string exec_type = "synchronous";
	int min_allowed_intersection = 1;
	
	// TODO: Add more commandlines
	clopts.attach_option("predictions", predictions,
	                   "The prefix (folder and filename) to save predictions.");
	clopts.attach_option("engine", exec_type, 
	                   "The engine type synchronous or asynchronous");
	clopts.attach_option("min_allowed_intersection", min_allowed_intersection,
						"The minimum number of common users that have rated two items for considering for similarity computation.")
	parse_implicit_command_line(clopts);

	if(!clopts.parse(argc, argv) /*|| input_dir == ""*/)
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
    graph.load("graph.txt", graph_loader);
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
    dc.cout() << "Calculating Item average and list of items to compare to...\n";
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

	// Run map_reduce on all the item_vertices to get the global sparse matrix of item vectors

	// Get the list of similar items and the
	dc.cout() << "Calcute the Top - k similar items for each item...\n";
	engine.set_update_function();
	engine.signal();

    // Save the predictions if indicated by the user
    if(!predictions.empty())
    {
	    std::cout << "Saving the Recommended Items for each User...\n";
	    const bool gzip_output = false;
	    const bool save_vertices = true;
	    const bool save_edges = false;
	    const size_t threads_per_machine = 2;

	    //save the predictions
	    graph.save(predictions, graph_writer(),
	               gzip_output, save_vertices, 
	               save_edges, threads_per_machine);

	    /*
	    //save the linear model
	    graph.save(predictions + ".U", linear_model_saver_U(),
			gzip_output, save_edges, save_vertices, threads_per_machine);
	    graph.save(predictions + ".V", linear_model_saver_V(),
			gzip_output, save_edges, save_vertices, threads_per_machine);
		*/
	}

	// Close MPI and return success
	graphlab::mpi_tools::finalize();
	return EXIT_SUCCESS;

}// end of main