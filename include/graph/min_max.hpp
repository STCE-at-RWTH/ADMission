#ifndef MIN_MAX_HPP
#define MIN_MAX_HPP

// ******************************** Includes ******************************** //

#include "admission_config.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/graph_traits.hpp>

#include <iterator>
#include <list>

// **************************** Header contents ***************************** //

namespace admission
{

/**
 * \addtogroup DagInfo
 * @{
 */

/**\brief Returns a list containing the minimal vertices.
 * @param g Const reference to the DAG.
 * @returns a list of all minimal vertices.
 */
template<typename Graph>
ADM_ALWAYS_INLINE std::list<
    typename boost::graph_traits<Graph>::vertex_descriptor>
min_vertices(const Graph& g)
{
  std::list<typename boost::graph_traits<Graph>::vertex_descriptor> min_vertices;

  BOOST_FOREACH(auto v, vertices(g))
  {
    if (in_degree(v, g) == 0 && out_degree(v, g) > 0)
    {
      min_vertices.push_back(v);
    }
  }
  return min_vertices;
}

/**\brief Returns a list of vertex_descriptors
 * which are maximal vertices.
 * @param g Const reference to the DAG.
 * @returns a list of all maximal vertices.
 */
template<typename Graph>
ADM_ALWAYS_INLINE std::list<
    typename boost::graph_traits<Graph>::vertex_descriptor>
max_vertices(const Graph& g)
{
  std::list<typename boost::graph_traits<Graph>::vertex_descriptor> max_vertices;

  BOOST_FOREACH(auto v, vertices(g))
  {
    if (out_degree(v, g) == 0 && in_degree(v, g) > 0)
    {
      max_vertices.push_back(v);
    }
  }
  return max_vertices;
}

/**\brief Returns a list of vertex_descriptors of the face graph g
 * which correspond to the non-intermediate edges in the graph from which g was obtained.
 * @param g Const reference to the DAG.
 * @returns a list of all non-intermediate edges.
 */
template<typename Graph>
ADM_ALWAYS_INLINE std::list<
    std::pair<typename boost::graph_traits<Graph>::vertex_descriptor, bool>>
corr_non_inter_edges(const Graph& g)
{
  std::list<
      std::pair<typename boost::graph_traits<Graph>::vertex_descriptor, bool>>
      non_inter_vertices;

  BOOST_FOREACH(auto v, vertices(g))
  {
    if (in_degree(v, g) && out_degree(v, g))
    {
      if (!in_degree(source(*in_edges(v, g).first, g), g))
      {
        non_inter_vertices.emplace_back(v, true);
      }
      else if (!out_degree(target(*out_edges(v, g).first, g), g))
      {
        non_inter_vertices.emplace_back(v, false);
      }
    }
  }
  return non_inter_vertices;
}

/**\brief Returns a list of vertex_descriptors of the face graph g
 * which correspond to the intermediate edges in the graph from which g was obtained.
 * @param g Const reference to the DAG.
 * @returns a list of all vertices corresponding to intermediate edges.
 */
template<typename Graph>
ADM_ALWAYS_INLINE std::list<
    typename boost::graph_traits<Graph>::vertex_descriptor>
corr_inter_edges(const Graph& g)
{
  std::list<typename boost::graph_traits<Graph>::vertex_descriptor>
      inter_vertices;

  BOOST_FOREACH(auto v, vertices(g))
  {
    if (in_degree(v, g) && out_degree(v, g))
    {
      if (in_degree(source(*in_edges(v, g).first, g), g) &&
          out_degree(target(*out_edges(v, g).first, g), g))
        inter_vertices.push_back(v);
    }
  }
  return inter_vertices;
}

/**
 * @}
 */

}  // end namespace admission

#endif  // MIN_MAX_HPP
