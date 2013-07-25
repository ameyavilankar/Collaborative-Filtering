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

//add 2 to negative node id to prevent -0 and -1 which are not allowed as vertex ids
const int SAFE_NEG_OFFSET = 2; 

// Type for the Item-id
typedef int item_id_type;

// Type to store the rating on the edge of the graph(can be changed to int for 0-5 range)
typedef double rating_type;

// Type for the list of items to be saved on an edge
typedef std::map<item_id_type, rating_type> rated_items_type;

/**
* \brief We use the rated_items type in accumulators and so we define an
* operator+= so that we can combine two rated_items into a single one
*/
inline rated_items_type& operator+=(rated_items_type& left, const rated_items_type& right)
{
	// Handle Self assignment
	if(left == right)
		return left;

	// Add entries from right if right is not empty
	if(!right.empty())
	{
		if(left.empty())
			left = right;
		else
		{
			for(rated_items_type::const_iterator it = right.begin(); it != right.end(); it++)
				left[it->first] = it->second;
		}
	}

	return left;
}

/**
* \brief This is used to find the intersection (common users) for two items in distance calculation
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

/*
* \brief The id of the user/item.
*/
typedef int id_type;

// This reperesents id on which gather apply step has not been applied
const id_type NULL_ID = -1;

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
 * \brief The vertex data represents each item and user in the
 * dataset and contains the rating given by each user to each movie/item.
 */
struct vertex_data
{
	// Stores the id on the movie/item
	item_id_type data_id;

	// Total number of times the vertex has been update...incremented everytime apply is run
	uint32_t num_updates;

	// The average rating of user..
	rating_type average_rating;

	// A Map that stores the user rating for an item
	rated_items_type rated_items;

	// recommended items
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
*  It also holds the list of items rated by a user.
*/
struct edge_data
{
	// The rating for an edge
	rating_type rating;

	// To hold the list of rated items
	// NOT NEEDED?? rated_items_type rated_items;

	// To hold the list of similar items
	// NOT NEEDED?? rated_items_type similar_items;

	// Constructor
	edge_data(rating_type rat = 0.0): rating(rat)/*, rated_items(), similar_items()*/ {}

	// Functions to make edge_data serializable
	void save(graphlab::oarchive& arc) const
	{
		// TODO rated_items saving
		arc << rating /* << rated_items << similar_items*/;
	}

	void load(graphlab::iarchive& arc)
	{
		// TODO rated_items loading
		arc >> rating /* >> rated_items >> similar_items*/;
	}

}; // End of the edge_data class


/**
 * \brief The Item Based graph is a bipartite graph with Items/Movies connected to
 * Users if the User has rated the Item/Movie.
 *
 * The edges store the rating given by the User to the Item/Movie.
 *
 * The vertices store the total topic counts.
 */
typedef graphlab::distributed_graph<vertex_data, edge_data> graph_type;

/**
 * \brief The graph loader is used by graph.load to parse lines of the
 * text data file.
 *
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
	return vertex.num_in_edges() > 0 ? 1 : 0;
}


/**
 * \brief Determine if the given vertex is a user vertex
 *
 * For simplicity we connect users --> items and therefore if a vertex
 * has out edges then it is a user
 */
inline bool is_user(const graph_type::vertex_type& vertex)
{
	return vertex.num_out_edges() > 0 ? 1 : 0;
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
	// To hold the average rating contributed by each edge
	rating_type rating;

	// This will be stored on the edge when we run scatter on the user
	rated_items_type items;

	// Constructor: Used to create using rating and an empty vector
	gather_type(rating_type rat = 0.0, rated_items_type ri = rated_items_type()): rating(rat), items() {}

	// Constructor: Used to create using rating and rated_items map containing only
	gather_type(rating_type rat, item_id_type id): rating(rat)
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
		// TODO
		this->rating += right.rating;
		this->items += right.items;

		return *this; 
	}
};


class user_vertex_program:
	public graphlab::ivertex_program<graph_type, gather_type>,
	public graphlab::IS_POD_TYPE
{
public:
	/* \brief Gather on all the IN_EDGES */
	edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const
	{
		return graphlab::IN_EDGES;
	}

	/* \brief Return the gather_type structure that is made up of the rating on 
	* the edge and the rated_items map that contains the item connected to the user
	* (other vertex)
	*/
	gather_type gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
	{
		return gather_type(edge.data().rating, edge.target().data().data_id);
	}

	/**
   	* \brief Calculate the average of all the ratings on the IN_EDGES.
   	*/
	void apply(icontext_type& context, vertex_type& vertex, const struct gather_type& total)
	{
		// Get the number of rated items by the users
		const size_t num_rated_items = vertex.num_in_edges();
		ASSERT_GT(num_rated_items, 0);

		// Get a reference to the vertex data
		vertex_data& vdata = vertex.data();

		// Increment the num_updates to the vertex by 1
		vdata.num_updates++;

		// Calculate and set the average user rating for the vertex
		vdata.average_rating = total.rating/num_rated_items;

		// Save the list of the rated items for the scatter step
		vdata.rated_items = total.items;
	}

	/* \brief Once we find the map of rated items in the apply step,
	* propogate the list on all of the edges so that it can be used by
	* the items on the other end of the edge.
	*/
	edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const
	{
		return graphlab::NO_EDGES;
    	//return graphlab::IN_EDGES;
  	}

  	/* \brief Save the map of rated items on each edge so that it can be used by
  	* the item on the other side of the edge.
  	*/
  	void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
  	{
  		//edge.data().rated_items = rated_items;
  	}

  	/*
  	NOT NEEDED
  	// Functions to make the class serializable
  	void save(graphlab::oarchive& arc)
  	{
  		arc << rated_items;
  	}

  	void load(graphlab::iarchive arc)
  	{
  		arc >> rated_items;
  	}

  	*/
};


class item_vertex_program:
	public graphlab::ivertex_program<graph_type, gather_type>,
	public graphlab::IS_POD_TYPE
{
public:
	/* \brief Gather on all the IN_EDGES */
	edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const
	{
		return graphlab::OUT_EDGES;
	}

	/* \brief Return the gather_type structure that is made up of the rating on 
	* the edge and the similar_items map that contains all the items rated by the
	* user on the other vertex of the edge.
	*/
	gather_type gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
	{
		return gather_type(edge.data().rating, edge.target().data().rated_items);
	}

	/**
   	* \brief Calculate the average of all the ratings on the IN_EDGES.
   	* Also aggregate the list of items rated by the user.
   	*/
	void apply(icontext_type& context, vertex_type& vertex, const struct gather_type& total)
	{
		// Get the number of rated items by the users
		const size_t num_users_rated = vertex.num_out_edges();
		ASSERT_GT(num_users_rated, 0);

		// Get a reference to the vertex data
		vertex_data& vdata = vertex.data();

		// Increment the num_updates to the vertex by 1
		vdata.num_updates++;

		// Calculate and set the average user rating for the vertex
		vdata.average_rating = total.rating/num_users_rated;

		// Save the list of the rated items for the scatter step
		vdata.rated_items = total.items;
	}

	/* \brief Once we find the map of rated items in the apply step,
	* propogate the list on all of the edges so that it can be used by
	* the items on the other end of the edge.
	*/
	edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const
	{
    	return graphlab::OUT_EDGES;
  	}

  	/* \brief Save the map of rated items on each edge so that it can be used by
  	* the item on the other side of the edge.
  	*/
  	void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
  	{
  		//edge.data().similar_items = similar_items;
  	}

  	/*
  	// Functions to make the class serializable
  	void save(graphlab::oarchive& arc)
  	{
  		arc << similar_items;
  	}

  	void load(graphlab::iarchive arc)
  	{
  		arc >> similar_items;
  	}
  	*/

};


//



// TODO: Define a different gather type that will contain all the totals and the similarity score.
// Refer the CF sequential code.
class get_recommendation_program:
	public graphlab::ivertex_program<graph_type, rated_items_type>
{
	/* \brief Gather on all the in_edges to get the union of the similar movies
	* 	that should be considered for recommendation.
	*/
	edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const
	{
		return graphlab::IN_EDGES;
	}

	/* \brief returns the map of the similar items to the item on the other end of the 
	* edge.
	*/
	rated_items_type gather_type(icontext_type& context, const vertex_data& vertex, edge_type& edge) const
	{
		// TODO Check
		return edge.source().data().rated_items;
	}
};

// Used to save the results to file
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
    
    // --------------------------------------------------
    // TODO: Replace sync with string
    dc.cout() << "Calculating User average and items rated by user...\n";
    graphlab::omni_engine<user_vertex_program> user_engine(dc, graph, exec_type, clopts);
    user_engine.signal_vset(user_set);
    user_engine.start();
    
    dc.cout() << "Calculating Item average and list of items to compare to...\n";
    graphlab::omni_engine<item_vertex_program> item_engine(dc, graph, exec_type, clopts);
    item_engine.signal_vset(item_set);
    item_engine.start();

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
