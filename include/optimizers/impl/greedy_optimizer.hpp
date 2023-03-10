#ifndef ADM_INC_OPTIMIZERS_IMPL_GREEDY_OPTIMIZER_HPP_
#define ADM_INC_OPTIMIZERS_IMPL_GREEDY_OPTIMIZER_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "optimizers/greedy_optimizer.hpp"  // IWYU pragma: keep

#include <stdexcept>

// ########################## FORWARD DECLARATIONS ########################## //

namespace admission
{
class LowerBound;
}

// ###################### TEMPLATE AND INLINE CONTENTS ###################### //

namespace admission
{

// ----- Greedy Optimizer - Override functions to query certain members ----- //

/******************************************************************************
 * @brief A GreedyOptimizer does not have a LowerBound.
 *
 * @returns false.
 ******************************************************************************/
ADM_ALWAYS_INLINE bool GreedyOptimizer::has_lower_bound()
{
  return false;
}

/******************************************************************************
 * @brief A GreedyOptimizer can not be parallelised.
 *
 * @returns false.
 ******************************************************************************/
ADM_ALWAYS_INLINE bool GreedyOptimizer::is_parallel()
{
  return false;
}

/******************************************************************************
 * @brief Calling set_lower_bound() throws an exception.
 ******************************************************************************/
ADM_ALWAYS_INLINE void GreedyOptimizer::set_lower_bound(admission::LowerBound&)
{
  throw std::runtime_error("LowerBound can not be set for an GreedyOptimizer.");
}

/******************************************************************************
 * @brief Calling set_lower_bound() throws an exception.
 *
 * @param[in] e Pointer to the lower bound.
 ******************************************************************************/
ADM_ALWAYS_INLINE void GreedyOptimizer::set_lower_bound(admission::LowerBound* e)
{
  set_lower_bound(*e);
}

/******************************************************************************
 * @brief Calling set_parallel_depth() throws an exception.
 ******************************************************************************/
ADM_ALWAYS_INLINE void GreedyOptimizer::set_parallel_depth(const plength_t)
{
  throw std::runtime_error("GreedyOptimizer is not parallel.");
}

/******************************************************************************
 * @brief We return a nullptr if asked for the LowerBound.
 *
 * @returns nullptr.
 ******************************************************************************/
ADM_ALWAYS_INLINE const LowerBound* GreedyOptimizer::get_lower_bound()
{
  return nullptr;
}

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_OPTIMIZERS_IMPL_GREEDY_OPTIMIZER_HPP_
