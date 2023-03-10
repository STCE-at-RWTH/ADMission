#ifndef ADM_INC_LOWER_BOUNDS_LOWER_BOUND_HPP_
#define ADM_INC_LOWER_BOUNDS_LOWER_BOUND_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "util/factory.hpp"

#include <functional>
#include <limits>
#include <vector>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

/******************************************************************************
 * @brief Base class for lower_bound to be used by branch and bound style
 *        algorithms. Provides a virtual operator() for computing the bound.
 ******************************************************************************/
class LowerBound
{
 public:
  //! Transitive propagation of the base class.
  typedef LowerBound AbstractBase;

  //! Default constructor of LowerBound.
  LowerBound() = default;

  //! Default destructor of LowerBound.
  virtual ~LowerBound() = default;

  //! Applies the lower bound to a face DAG.
  virtual flop_t operator()(const FaceDAG&) const;

 protected:
  /****************************************************************************
   * @brief We use a struct to encapsule wether we want
   *        numeric_limits<T>::max() or numeric_limits<T>::min() of a type T.
   *
   * @tparam Comp Comparison functor.
   * @tparam T Type of the functor arguments.
   ****************************************************************************/
  template<typename Comp, typename T>
  struct Extreme
  {};

  /****************************************************************************
   * @brief Using Extreme<Comp, T> with comp == std::less
   *        will set value the maximum possible value of T.
   *
   * @tparam T Type of the functor arguments.
   ****************************************************************************/
  template<typename T>
  struct Extreme<std::less<>, T>
  {
    //! Extreme value (maximal representable number of type T)
    static constexpr T value = std::numeric_limits<T>::max();
  };

  /****************************************************************************
   * @brief Using Extreme<Comp, T> with comp == std::greater
   *        will set value the minumum possible value of T.
   *
   * @tparam T Type of the functor arguments.
   ****************************************************************************/
  template<typename T>
  struct Extreme<std::greater<>, T>
  {
    //! Extreme value (minimal representable number of type T)
    static constexpr T value = std::numeric_limits<T>::min();
  };

  //! Helper function for derived classes that need to search for smallest
  //! edge-sizes within the upper or lower induced DAGs of an edge.
  template<
      typename Graph, typename EdgeDesc, typename Acc = lower_accessor,
      typename Comp = std::less<>, typename Property = boost::edge_size_t>
  void put_extreme(
      const Graph& g, const EdgeDesc& s, std::vector<flop_t>& v,
      Acc a = lower_accessor(), Comp c = std::less<>(),
      Property p = boost::edge_size) const;
};

//! Declare the LowerBoundFactory.
using LowerBoundFactory = admission::Factory<LowerBound>;

}  // end namespace admission

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "lower_bounds/impl/lower_bound.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_LOWER_BOUNDS_LOWER_BOUND_HPP_
