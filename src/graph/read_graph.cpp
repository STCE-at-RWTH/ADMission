// ******************************** Includes ******************************** //

#include "graph/read_graph.hpp"

#include "admission_config.hpp"

#include <boost/any.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphml.hpp>
#include <boost/graph/properties.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_map/dynamic_property_map.hpp>

#include <filesystem>
#include <fstream>
#include <string>
#include <utility>

// **************************** Source contents ***************************** //

namespace admission
{

void read_mmchain(std::ifstream& stream, admission::DAG& g)
{
  // To store the number of edges (number of matrixes in the chaining problem).
  size_t ne;
  // Buffer to store the whitespace-separated inputs to guarantee correct conversion to integers.
  std::string buf;

  auto n = boost::get(boost::vertex_size, g);
  auto Fbardot_exists = boost::get(boost::edge_has_model, g);
  auto Fprime_exists = boost::get(boost::edge_acc_stat, g);
  auto c_tan = boost::get(boost::edge_c_tan, g);
  auto c_adj = boost::get(boost::edge_c_adj, g);

  stream >> buf;
  ne = std::stoul(buf);

  // Add the vertex before the first edge.
  VertexDesc thisv, lastv = add_vertex(g);

  for (size_t i = 0; i < ne; ++i)
  {
    thisv = add_vertex(g);
    auto [e, succ] = add_edge(lastv, thisv, g);
    Fprime_exists[e] = false;
    Fbardot_exists[e] = true;

    stream >> buf;
    n[thisv] = std::stoul(buf);
    stream >> buf;

    if (i == 0)
    {
      n[lastv] = std::stoul(buf);
    }
    else if (n[lastv] != std::stol(buf))
    {
      throw matrix_dim_mismatch(
          std::stoul(buf), n[lastv],
          std::string(" at ") + std::to_string(stream.tellg()) +
              "th character of input!");
    }

    stream >> buf;
    c_adj[e] = c_tan[e] = std::stoul(buf);

    lastv = thisv;
  }
}

/**
 * The read_graphml overload in admission
 * works by linking different properties defined
 * in the graphml file to the boost::graph_opt_cost
 * property of the graph instance,
 * So we can read a graph annotated with information
 * for vector functions and a graph for scalar functions
 * from the same file and specitfy the optimal solutions
 * for each category.
 */
void read_graphml(std::ifstream& stream, admission::DAG& g)
{
  boost::dynamic_properties dp(boost::ignore_other_properties);

  dp.property("size", boost::get(boost::vertex_size, g));

  dp.property("acc_stat", boost::get(boost::edge_acc_stat, g));
  dp.property("has_model", boost::get(boost::edge_has_model, g));
  dp.property("c_tan", boost::get(boost::edge_c_tan, g));
  dp.property("c_adj", boost::get(boost::edge_c_adj, g));

  boost::read_graphml(stream, g, dp);
}

/**
 * The read_graphml overload in admission
 * works by linking different properties defined
 * in the graphml file to the boost::graph_opt_cost
 * property of the graph instance,
 * So we can read a graph annotated with information
 * for vector functions and a graph for scalar functions
 * from the same file and specitfy the optimal solutions
 * for each category.
 */
void read_graphml(std::ifstream& stream, admission::FaceDAG& g)
{
  boost::dynamic_properties dp(boost::ignore_other_properties);

  dp.property("edge_index", boost::get(boost::edge_index, g));
  dp.property("edge_size", boost::get(boost::edge_size, g));

  dp.property("c_tan", boost::get(boost::vertex_c_tan, g));
  dp.property("c_adj", boost::get(boost::vertex_c_adj, g));
  dp.property("has_model", boost::get(boost::vertex_has_model, g));
  dp.property("acc_stat", boost::get(boost::vertex_acc_stat, g));

  boost::read_graphml(stream, g, dp);
}

/**Tries to read as a mtrix chain first. If the input does not start with a valid unsigned integer
 * the reading is aborted and the file is parsed as a GRAPHML file.
 */
void read_graph(fs::path p, admission::DAG& g)
{
  g.clear();
  namespace fs = std::filesystem;

  (pattern_read_path = p).remove_filename();
  if (!fs::exists(p))
  {
    throw std::runtime_error(p.string() + " is not a file!");
  }
  std::ifstream i(p);
  if (!i.good())
  {
    throw std::runtime_error("The file " + p.string() + " could not be read!");
  }

  try
  {
    read_mmchain(i, g);
  }
  catch (matrix_dim_mismatch& e)
  {
    throw e;
  }
  catch (std::logic_error& e)
  {
    i.seekg(0);
    g.clear();
    read_graphml(i, g);
  }
}

}  // end namespace admission
