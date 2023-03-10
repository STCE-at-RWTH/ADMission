#ifndef ADM_INC_LOWER_BOUNDS_SIMPLE_MIN_ACC_COST_BOUND_HPP_
#define ADM_INC_LOWER_BOUNDS_SIMPLE_MIN_ACC_COST_BOUND_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "lower_bounds/lower_bound.hpp"
#include "util/factory.hpp"

#include <map>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

/******************************************************************************
 * @brief Computes a lower bound for JA on a face DAG by undercutting the
 *        cost for accumulating unaccumulated Jacobians.
 *
 * @returns 0 if no elemental Jacobians can be preaccumulated.
 ******************************************************************************/
class SimpleMinAccCostBound : public LowerBound
{
 public:
  //! Make base class name available.
  typedef LowerBound Base;
  //! Transitive propagation of the base class.
  typedef typename Base::AbstractBase AbstractBase;

  //! Default constructor of SimpleMinAccCostBound.
  SimpleMinAccCostBound() = default;

  //! Default destructor of SimpleMinAccCostBound.
  ~SimpleMinAccCostBound() = default;

  //! Applies the lower bound to a face DAG.
  virtual flop_t operator()(const FaceDAG& g) const final;
};

ADM_REGISTER_TYPE(SimpleMinAccCostBound, SimpleMinAccCostBound)

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_LOWER_BOUNDS_SIMPLE_MIN_ACC_COST_BOUND_HPP_
