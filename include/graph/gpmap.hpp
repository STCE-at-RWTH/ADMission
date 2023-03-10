#ifndef ADM_INC_GRAPH_GPMAP_HPP_
#define ADM_INC_GRAPH_GPMAP_HPP_

// ################################ INCLUDES ################################ //

#include "misc/doxygen.hpp"

#include <boost/property_map/property_map.hpp>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(Misc)

/******************************************************************************
 * @brief Mock up for a property map, which we will use to
 *        read and write properties of graphs.
 *
 * Needed to do this, because Boost.Graph might be peer-reviewed,
 * but is certainly not well documented :(
 *
 * @tparam Graph Type of the graph we operate on.
 * @tparam Tag A class or enum type used to identify the property.
 * @tparam Value Underlying value type of Tag. This is the nasty part.
 ******************************************************************************/
template<typename Graph, typename Tag, typename Value>
class graph_pmap
{
 public:
  DOXYGEN_GROUP_BEGIN(Typedefs mimicking the boost::property_map, )

  typedef Graph* key_type;
  typedef Value value_type;
  typedef boost::lvalue_property_map_tag category;
  typedef Tag tag_type;

  DOXYGEN_GROUP_END()
};

}  // end namespace admission

namespace boost
{

/******************************************************************************
 * @brief Overload of the property traits in namespace boost.
 *
 * read_graphml requires this.
 *
 * @tparam Graph Type of the graph we operate on.
 * @tparam Tag A class or enum type used to identify the property.
 * @tparam Value Underlying value type of Tag. This is the nasty part.
 ******************************************************************************/
template<typename Graph, typename Tag, typename Value>
struct property_traits<admission::graph_pmap<Graph, Tag, Value>>
{
  //! Make value_type available as reference.
  using reference =
      typename admission::graph_pmap<Graph, Tag, Value>::value_type;

  //! Make key type available.
  using key_type = typename admission::graph_pmap<Graph, Tag, Value>::key_type;

  //! Make value type available.
  using value_type =
      typename admission::graph_pmap<Graph, Tag, Value>::value_type;

  //! Make category type available.
  using category = typename admission::graph_pmap<Graph, Tag, Value>::category;
};

//! Overloaded get, using a pointer to graph as selector
//! and delegating to get_property defined for adjacency_list.
template<typename Value, typename Graph, typename Tag>
Value get(const admission::graph_pmap<Graph, Tag, Value>&, Graph* gp);

//! Overloaded put, using a pointer to graph as selector
//! and delegating to set_property defined for adjacency_list.
template<typename Graph, typename Tag, typename Value>
void put(const admission::graph_pmap<Graph, Tag, Value>&, Graph* gp, Value val);

//! Make-function allowing an at least somewhat like boost::get
//! syntax by determining some of the types from its argumentes.
template<typename Value, typename Graph, typename Tag>
admission::graph_pmap<Graph, Tag, Value> make_gpmap(const Tag, const Graph&);

DOXYGEN_MODULE_END(Misc)

}  // end namespace boost

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "graph/impl/gpmap.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_GRAPH_GPMAP_HPP_
