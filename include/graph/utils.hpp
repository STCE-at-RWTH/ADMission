#ifndef GRAPH_UTILS_HPP
#define GRAPH_UTILS_HPP

// ******************************** Includes ******************************** //

#include "admission_config.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/graph_traits.hpp>

// **************************** Header contents ***************************** //

namespace admission
{

/**
 * \defgroup DagMod  Modifying DAGs and FaceDAGs.
 * \defgroup DagInfo Information about the structure of DAGs and FaceDAGs.
 */

/**\ingroup DagInfo
 * \brief Computes the longest directed path (...,v) ending in v.
 *
 * Complexity O(|E|).
 * @tparam Graph Type of the graph.
 * @param[in] g Reference to the graph.
 * @param[in] v Vertex descriptor of the vertex to start from.
 * @returns The longest reverse path starting form a vertex.
 */
template<typename Graph>
ADM_STRONG_INLINE plength_t longest_reverse_path_from(
    Graph& g, typename boost::graph_traits<Graph>::vertex_descriptor v)
{
  plength_t l_path = 0;
  plength_t local_l_path = 0;

  if (in_degree(v, g) == 0)
  {
    return 0;
  }

  BOOST_FOREACH(auto e, in_edges(v, g))
  {
    local_l_path = longest_reverse_path_from(g, source(e, g));
    l_path = (local_l_path > l_path) ? local_l_path : l_path;
  }
  return 1 + l_path;
}

/**\ingroup DagInfo
 * \brief Computes the longest directed path (v,...) starting with v.
 *
 * Complexity O(|E|).
 * @tparam Graph Type of the graph.
 * @param[in] g Reference to the graph.
 * @param[in] v Vertex descriptor of the vertex to start from.
 * @returns The longest path starting form a vertex.
 */
template<typename Graph>
ADM_STRONG_INLINE plength_t longest_path_from(
    Graph& g, typename boost::graph_traits<Graph>::vertex_descriptor v)
{
  plength_t l_path = 0;
  plength_t local_l_path = 0;

  if (out_degree(v, g) == 0)
  {
    return 0;
  }

  BOOST_FOREACH(auto e, out_edges(v, g))
  {
    local_l_path = longest_path_from(g, target(e, g));
    l_path = (local_l_path > l_path) ? local_l_path : l_path;
  }
  return 1 + l_path;
}

/**\ingroup DagInfo
 * \brief Computes the longest directed path (...,v,...) including v.
 *
 * Complexity O(|E|).
 * @tparam Graph Type of the graph.
 * @param g Reference to the graph.
 * @param v Vertex descriptor included in the path.
 * @returns Length of the longest path including v.
 */
template<typename Graph>
ADM_ALWAYS_INLINE plength_t longest_bidirectional_path_from(
    Graph& g, typename boost::graph_traits<Graph>::vertex_descriptor v)
{
  return longest_path_from(g, v) + longest_reverse_path_from(g, v);
}

/**\ingroup DagInfo
 * \brief Computes the length of the longest
 * directed path in a (face) DAG.
 *
 * Complexity O(?).
 * @returns The longest path inside the Graph.
 * @tparam Graph Type of the graph.
 * @param[in] g Reference to the graph.
 */
template<typename GraphT>
ADM_ALWAYS_INLINE plength_t longest_path(GraphT& g)
{
  plength_t l_path = 0;

  for (auto v = vertices(g); v.first != v.second; v.first++)
  {
    plength_t local_l_path = longest_path_from(g, *v.first);
    l_path = (local_l_path > l_path) ? local_l_path : l_path;
  }
  return l_path;
}

}  // end namespace admission

#endif  // GRAPH_UTILS_HPP
