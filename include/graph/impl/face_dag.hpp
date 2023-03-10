#ifndef ADM_INC_GRAPH_IMPL_FACE_DAG_HPP_
#define ADM_INC_GRAPH_IMPL_FACE_DAG_HPP_

// ################################ INCLUDES ################################ //

#include "graph/face_dag.hpp"  // IWYU pragma: keep

#include <boost/foreach.hpp>
#include <boost/graph/properties.hpp>

#include <limits>

// ###################### TEMPLATE AND INLINE CONTENTS ###################### //

namespace admission
{

/******************************************************************************
 * @brief Finds the global mininmal vertex index inside a DAG.
 *
 * @tparam Graph Type of the graph.
 * @param[in] g Reference to the (face) DAG.
 * @returns The index of the minimal vertex.
 ******************************************************************************/
template<typename Graph>
ADM_ALWAYS_INLINE index_t min_vertex_index(const Graph& g)
{
  index_t min = std::numeric_limits<index_t>::max();
  BOOST_FOREACH(auto v, vertices(g))
  {
    min = boost::get(boost::vertex_index, g, v) < min ?
              boost::get(boost::vertex_index, g, v) :
              min;
  }
  return min;
}

/******************************************************************************
 * @brief Finds the global maximal vertex index inside a DAG.
 *
 * @tparam Graph Type of the graph.
 * @param[in] g Reference to the (face) DAG.
 * @returns The index of the maximal vertex.
 ******************************************************************************/
template<typename Graph>
ADM_ALWAYS_INLINE index_t max_vertex_index(const Graph& g)
{
  index_t max = std::numeric_limits<index_t>::lowest();
  BOOST_FOREACH(auto v, vertices(g))
  {
    max = boost::get(boost::vertex_index, g, v) > max ?
              boost::get(boost::vertex_index, g, v) :
              max;
  }
  return max;
}

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_GRAPH_IMPL_FACE_DAG_HPP_
