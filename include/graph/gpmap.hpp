#ifndef GPMAP_HPP
#define GPMAP_HPP

// ******************************** Includes ******************************** //

#include <boost/property_map/property_map.hpp>

// **************************** Header contents ***************************** //

namespace admission
{

/**
 * \defgroup Misc Miscellaneous overloads and additions to external libs.
 * \addtogroup Misc
 * @{
 */

/**\brief Mock up for a property map, which we will use to
 *        read and write properties of graphs.
 *
 * Needed to do this, because Boost.Graph might be peer-reviewed,
 * but is certainly not well documented :(
 *
 * @tparam Graph Type of the graph we operate on
 * @tparam Tag A class or enum type used to identify the property
 * @tparam Value Underlying value type of Tag. This is the nasty part.
 */
template<typename Graph, typename Tag, typename Value>
class graph_pmap
{
 public:
  ///\name Typedefs mimicking the boost::propery_map.
  ///@{
  typedef Graph* key_type;
  typedef Value value_type;
  typedef boost::lvalue_property_map_tag category;
  typedef Tag tag_type;
  ///@}
};

}  // end namespace admission

namespace boost
{

/// Overload of the property traits in namespace boost. read_graphml requires this.
template<typename Graph, typename Tag, typename Value>
struct property_traits<admission::graph_pmap<Graph, Tag, Value>>
{
  /// Make value_type available as reference.
  using reference =
      typename admission::graph_pmap<Graph, Tag, Value>::value_type;

  /// Make key type available.
  using key_type = typename admission::graph_pmap<Graph, Tag, Value>::key_type;

  /// Make value type available.
  using value_type =
      typename admission::graph_pmap<Graph, Tag, Value>::value_type;

  /// Make category type available.
  using category = typename admission::graph_pmap<Graph, Tag, Value>::category;
};

/**\brief Overloaded get, using a pointer to graph as selector
 *        and delegating to get_property defined for adjacency_list.
 *
 * @tparam Value Value type stored as a property of Graph.
 * @tparam Graph Type of the graph.
 * @tparam Tag   Enum type identifying the property.
 * @param gp Reference to a graph_pmap<...> pointing to a property of a Graph.
 * @returns Value The value pointed to by the graph_pmap<...>.
 */
template<typename Value, typename Graph, typename Tag>
Value get(const admission::graph_pmap<Graph, Tag, Value>&, Graph* gp)
{
  return get_property(*gp, Tag());
}

/**\brief Overloaded put, using a pointer to graph as selector
 *        and delegating to set_property defined for adjacency_list
 *
 * @tparam Value Value type stored as a property of Graph.
 * @tparam Graph Type of the graph.
 * @tparam Tag   Enum type identifying the property.
 * @param gp Reference to a graph_pmap<...> pointing to a property of a Graph.
 * @param val Value to be stored.
 */
template<typename Graph, typename Tag, typename Value>
void put(const admission::graph_pmap<Graph, Tag, Value>&, Graph* gp, Value val)
{
  set_property(*gp, Tag(), val);
}

/**\brief Make-function allowing an at least somewhat like    boost::get
 *        syntax by determining some of the types from its argumentes.
 *
 * @tparam Value Value type stored as a property of Graph.
 * @tparam Graph Type of the graph.
 * @tparam Tag   Enum type identifying the property.
 * @param g Const reference to the Graph the property belongs to.
 * @param t Enum value providing type-safe association by the enum type Tag.
 * @returns graph_pmap<...>.
 */
template<typename Value, typename Graph, typename Tag>
admission::graph_pmap<Graph, Tag, Value> make_gpmap(const Tag, const Graph&)
{
  return admission::graph_pmap<Graph, Tag, Value>();
}

/**
 * @}
 */

}  // end namespace boost
#endif  // GPMAP_HPP
