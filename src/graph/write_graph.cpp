// ################################ INCLUDES ################################ //

#include "graph/write_graph.hpp"

#include "graph/chain.hpp"

#include <boost/exception/exception.hpp>
#include <boost/exception/info.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphml.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/dynamic_property_map.hpp>

#include <algorithm>
#include <memory>
#include <ostream>
#include <tuple>

// ############################ SOURCE CONTENTS ############################# //

namespace admission
{

namespace detail
{

/******************************************************************************
 * @brief Writes an admission::DAG to a file in GRAPHML format.
 *
 * @param[inout] os Reference to the ostream to write to.
 * @param[in] g Reference to the DAG.
 ******************************************************************************/
void write_graphml(std::ostream& os, admission::DAG& g)
{
  boost::dynamic_properties dp;

  dp.property("has_model", boost::get(boost::edge_has_model, g));
  dp.property("index", boost::get(boost::vertex_index, g));
  dp.property("size", boost::get(boost::vertex_size, g));

  dp.property("tangent_cost", boost::get(boost::edge_c_tan, g));
  dp.property("adjoint_cost", boost::get(boost::edge_c_adj, g));
  dp.property("has_jacobian", boost::get(boost::edge_acc_stat, g));

  boost::write_graphml(os, g, dp);
}

/******************************************************************************
 * @brief Writes an admission::FaceDAG to a file in GRAPHML format.
 *
 * @param[inout] os Reference to the ostream to write to.
 * @param[in] g Reference to the DAG.
 ******************************************************************************/
void write_graphml(std::ostream& os, admission::FaceDAG& g)
{
  boost::dynamic_properties dp;

  dp.property("edge_index", boost::get(boost::edge_index, g));
  dp.property("edge_size", boost::get(boost::edge_size, g));

  dp.property("index", boost::get(boost::vertex_index, g));
  dp.property("tangent_cost", boost::get(boost::vertex_c_tan, g));
  dp.property("adjoint_cost", boost::get(boost::vertex_c_adj, g));
  dp.property("has_model", boost::get(boost::vertex_has_model, g));
  dp.property("has_jacobian", boost::get(boost::vertex_acc_stat, g));

  boost::write_graphml(os, g, dp);
}

}  // end namespace detail

/******************************************************************************
 * @brief Writes an admission::DAG to a file as mmchain.
 *
 * @param[inout] os Reference to the ostream to write to.
 * @param[in] g Reference to the DAG.
 ******************************************************************************/
void write_mmchain(std::ostream& os, admission::DAG& g)
{
  std::shared_ptr<DAG> c = make_chain(g);
  auto n = boost::get(boost::vertex_size, *c);
  auto c_tan = boost::get(boost::edge_c_tan, *c);
  auto c_adj = boost::get(boost::edge_c_adj, *c);
  VertexDesc l;
  EdgeDesc e;

  os << num_edges(*c) << "\n";
  for (l = 1; l < num_vertices(*c); ++l)
  {
    std::tie(e, std::ignore) = boost::edge(l - 1, l, *c);
    os << n[l] << " " << n[l - 1] << " " << std::max(c_tan[e], c_adj[e])
       << "\n";
  }
}

}  // end namespace admission

// ################################## EOF ################################### //
