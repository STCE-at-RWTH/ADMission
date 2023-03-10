#ifndef ADM_INC_GRAPH_IMPL_GPMAP_HPP_
#define ADM_INC_GRAPH_IMPL_GPMAP_HPP_

// ################################ INCLUDES ################################ //

#include "graph/gpmap.hpp"  // IWYU pragma: keep

// ###################### TEMPLATE AND INLINE CONTENTS ###################### //

namespace boost
{

/******************************************************************************
 * @brief Overloaded get, using a pointer to graph as selector
 *        and delegating to get_property defined for adjacency_list.
 *
 * @tparam Value Value type stored as a property of Graph.
 * @tparam Graph Type of the graph.
 * @tparam Tag Enum type identifying the property.
 * @param gp Reference to a graph_pmap<...> pointing to a property of a Graph.
 * @returns Value The value pointed to by the graph_pmap<...>.
 ******************************************************************************/
template<typename Value, typename Graph, typename Tag>
Value get(const admission::graph_pmap<Graph, Tag, Value>&, Graph* gp)
{
  return get_property(*gp, Tag());
}

/******************************************************************************
 * @brief Overloaded put, using a pointer to graph as selector
 *        and delegating to set_property defined for adjacency_list
 *
 * @tparam Value Value type stored as a property of Graph.
 * @tparam Graph Type of the graph.
 * @tparam Tag   Enum type identifying the property.
 * @param gp Reference to a graph_pmap<...> pointing to a property of a Graph.
 * @param val Value to be stored.
 ******************************************************************************/
template<typename Graph, typename Tag, typename Value>
void put(const admission::graph_pmap<Graph, Tag, Value>&, Graph* gp, Value val)
{
  set_property(*gp, Tag(), val);
}

/******************************************************************************
 * @brief Make-function allowing an at least somewhat like boost::get
 *        syntax by determining some of the types from its argumentes.
 *
 * @tparam Value Value type stored as a property of Graph.
 * @tparam Graph Type of the graph.
 * @tparam Tag   Enum type identifying the property.
 * @param g Const reference to the Graph the property belongs to.
 * @param t Enum value providing type-safe association by the enum type Tag.
 * @returns graph_pmap<...>.
 ******************************************************************************/
template<typename Value, typename Graph, typename Tag>
admission::graph_pmap<Graph, Tag, Value> make_gpmap(const Tag, const Graph&)
{
  return admission::graph_pmap<Graph, Tag, Value>();
}

}  // end namespace boost

// ################################## EOF ################################### //

#endif  // ADM_INC_GRAPH_IMPL_GPMAP_HPP_
