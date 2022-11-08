#ifndef SIMPLE_MIN_ACC_COST_BOUND_HPP
#define SIMPLE_MIN_ACC_COST_BOUND_HPP

// ******************************** Includes ******************************** //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "lower_bounds/lower_bound.hpp"
#include "factory.hpp"

// **************************** Header contents ***************************** //

namespace admission
{

/**\brief Computes a lower bound for JA on a face DAG by undercutting the
 *        cost for accumulating unaccumulated Jacobians.
 *
 * Returns 0 if no elemental Jacobians can be preaccumulated.
 */
class SimpleMinAccCostBound : public LowerBound
{
 public:
  /// Make base class name available.
  typedef LowerBound Base;
  /// Transitive propagation of the base class.
  typedef typename Base::AbstractBase AbstractBase;

  ///\name Constructors and Destructor.
  ///@{
  SimpleMinAccCostBound() = default;

  ~SimpleMinAccCostBound() {}

  ///@}

  /**\brief Applies the lower bound to a face DAG.
   *
   * @param g Reference to the DAG we compute the bound on.
   * @returns A valid lower bound for matrix-free FE on g.
   */
  virtual flop_t operator()(const FaceDAG& g) const final;
};

ADM_REGISTER_TYPE(SimpleMinAccCostBound, SimpleMinAccCostBound);

}  // end namespace admission

#endif  // SIMPLE_MIN_ACC_COST_BOUND_HPP
