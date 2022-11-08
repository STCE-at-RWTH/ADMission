#ifndef LOWER_BOUND_HPP
#define LOWER_BOUND_HPP

// ******************************** Includes ******************************** //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "factory.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>

#include <functional>
#include <limits>
#include <vector>

// **************************** Header contents ***************************** //

namespace admission
{

/**\brief Base class for lower_bound to be used by branch and bound style algorithms.
 *        Provides a virtual operator() for computing the bound.
 */
class LowerBound
{
 public:
  /// Transitive propagation of the base class.
  typedef LowerBound AbstractBase;

  ///\name Constructors and Destructor.
  ///@{
  LowerBound() = default;

  virtual ~LowerBound() {}

  ///@}

  /**\brief Applies the lower bound to a face DAG.
   *
   * Just returns 0 in case of the base class and
   * is overwritten by derived classes.
   * @param g Reference to the DAG we compute the bound on.
   * @returns 0 (which is a valid lower bound for JA on any face DAG).
   */
  virtual flop_t operator()(const FaceDAG&) const
  {
    return 0;
  }

 protected:
  /**\brief We use a struct to encapsule wether we
   *        want numeric_limits<T>::max()
   *        or   numeric_limits<T>::min() of a type T.
   *  @tparam Comp Comparison functor.
   *  @tprarm T    Type of the functor arguments.
   */
  template<typename Comp, typename T>
  struct Extreme
  {};

  /**\brief Using Extreme<Comp, T> with comp == std::less
   *        will set value the maximum possible value of T.
   */
  template<typename T>
  struct Extreme<std::less<>, T>
  {
    static constexpr T value = std::numeric_limits<T>::max();
  };

  /**\brief Using Extreme<Comp, T> with comp == std::greater
   *        will set value the minumum possible value of T.
   */
  template<typename T>
  struct Extreme<std::greater<>, T>
  {
    static constexpr T value = std::numeric_limits<T>::min();
  };

  /**\brief Helper function for derived classes that need to search for smallest
   *        edge-sizes within the upper or lower induced DAGs of an edge.
   *
   * Simplified minimal Markowitz
   * degree bounds use this, as well as the SimpleMinAccCostBound.
   * Recursively calls itself on the neighbors of e.
   * @tparam Graph    Type of the (face) DAG
   * @tparam EdgeDesc Type of the EdgeDesc of an edge e in g
   * @tparam Acc      \ref lower_accessor or \ref upper_accessor -> Are we operating on a lower or upper induced sub-DAG?
   * @tparam Comp     std::less or std::greater -> are we searching for a min or max value?
   * @tparam Property Property of the edges we want to compare. Defaults to boost::edge_size_t.
   *
   * @param[in] g    A const ref to the (face) DAG.
   * @param[in] e    The EdgeDesc of the edge that induces the sub-DAG.
   * @param[inout] v Stores the extreme values for each vertex in the induced sub-DAG.
   * @param[in] a    Instance of the accessor.
   * @param[in] c    Instance of the comparator.
   * @param[in] p    Instance of the property.
   */
  template<
      typename Graph, typename EdgeDesc, typename Acc = lower_accessor,
      typename Comp = std::less<>, typename Property = boost::edge_size_t>
  void put_extreme(
      const Graph& g, const EdgeDesc& s, std::vector<flop_t>& v,
      Acc a = lower_accessor(), Comp c = std::less<>(),
      Property p = boost::edge_size) const
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
};

/// Declare the LowerBoundFactory
using LowerBoundFactory = admission::Factory<LowerBound>;

}  // end namespace admission

#endif  // LOWER_BOUND_HPP
