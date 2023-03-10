#ifndef ADM_INC_LOWER_BOUNDS_IMPL_LOWER_BOUND_HPP_
#define ADM_INC_LOWER_BOUNDS_IMPL_LOWER_BOUND_HPP_

// ################################ INCLUDES ################################ //

#include "lower_bounds/lower_bound.hpp"  // IWYU pragma: keep

#include <boost/foreach.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>

// ###################### TEMPLATE AND INLINE CONTENTS ###################### //

namespace admission
{

/******************************************************************************
 * @brief Helper function for derived classes that need to search for smallest
 *        edge-sizes within the upper or lower induced DAGs of an edge.
 *
 * Simplified minimal Markowitz degree bounds use this, as well as the
 * SimpleMinAccCostBound. Recursively calls itself on the neighbors of e.
 *
 * @tparam Graph    Type of the (face) DAG
 * @tparam EdgeDesc Type of the EdgeDesc of an edge e in g
 * @tparam Acc      \ref lower_accessor or \ref upper_accessor -> Are we
 *                  operating on a lower or upper induced sub-DAG?
 * @tparam Comp     std::less or std::greater -> are we searching for
 *                  a min or max value?
 * @tparam Property Property of the edges we want to compare. Defaults to
 *                  boost::edge_size_t.
 *
 * @param[in] g    A const ref to the (face) DAG.
 * @param[in] e    The EdgeDesc of the edge that induces the sub-DAG.
 * @param[inout] v Stores the extreme values for each vertex in the
 *                 induced sub-DAG.
 * @param[in] a    Instance of the accessor.
 * @param[in] c    Instance of the comparator.
 * @param[in] p    Instance of the property.
 ******************************************************************************/
template<
    typename Graph, typename EdgeDesc, typename Acc, typename Comp,
    typename Property>
void LowerBound::put_extreme(
    const Graph& g, const EdgeDesc& s, std::vector<flop_t>& v, Acc a, Comp c,
    Property p) const
{
  static_assert(
      (std::is_same_v<Comp, std::greater<>> ||
       std::is_same_v<Comp, std::less<>> == true),
      "Must be used with std::less<> or std::greater<>!");
  auto m = boost::make_iterator_property_map(
      v.begin(), boost::get(boost::vertex_index, g));

  if (c(m[a.next_vertex(s, g)], Extreme<Comp, flop_t>::value))
  {
    return;
  }

  m[a.next_vertex(s, g)] = boost::get(p, g, s);

  BOOST_FOREACH(auto ie, a.next_edges(s, g))
  {
    put_extreme(g, ie, v, a, c);
    if (c(m[a.next_vertex(ie, g)], m[a.next_vertex(s, g)]))
    {
      m[a.next_vertex(s, g)] = m[a.next_vertex(ie, g)];
    }
  }
}

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_LOWER_BOUNDS_IMPL_LOWER_BOUND_HPP_
