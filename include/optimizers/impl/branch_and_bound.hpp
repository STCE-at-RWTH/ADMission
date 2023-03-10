#ifndef ADM_INC_OPTIMIZERS_IMPL_BRANCH_AND_BOUND_HPP_
#define ADM_INC_OPTIMIZERS_IMPL_BRANCH_AND_BOUND_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "operations/op_sequence.hpp"
#include "optimizers/branch_and_bound.hpp"  // IWYU pragma: keep
#include "optimizers/optimizer.hpp"
#include "optimizers/optimizer_stats.hpp"

#include <iosfwd>

// ########################## FORWARD DECLARATIONS ########################## //

namespace admission
{
class LowerBound;
}

// ###################### TEMPLATE AND INLINE CONTENTS ###################### //

namespace admission
{

/******************************************************************************
 * @brief Default contructor of BranchAndBound.
 ******************************************************************************/
ADM_ALWAYS_INLINE BranchAndBound::BranchAndBound()
    : _lbound(nullptr), _parallel_depth(1), _glob_opt_s(OpSequence::make_max())
{}

/******************************************************************************
 * @brief Contructor of BranchAndBound with a given lower bound.
 *
 * @param[in] lbound Reference to the lower bound.
 ******************************************************************************/
ADM_ALWAYS_INLINE BranchAndBound::BranchAndBound(const LowerBound& lbound)
    : _lbound(&lbound), _parallel_depth(1), _glob_opt_s(OpSequence::make_max())
{}

// ------------------ BranchAndBound - Optimizer settings ------------------- //

/******************************************************************************
 * @brief Reset the optimal soluton cost and the counters.
 ******************************************************************************/
ADM_ALWAYS_INLINE void BranchAndBound::reset()
{
  _glob_opt_s = OpSequence::make_max();
  Base::reset();
}

// ------ BranchAndBound - Override functions to query certain members ------ //

/******************************************************************************
 * @brief BranchAndBound has a LowerBound.
 *
 * @returns true.
 ******************************************************************************/
ADM_ALWAYS_INLINE bool BranchAndBound::has_lower_bound()
{
  return true;
}

/******************************************************************************
 * @brief BranchAndBound can run in parallel.
 *
 * @returns true.
 ******************************************************************************/
ADM_ALWAYS_INLINE bool BranchAndBound::is_parallel()
{
  return true;
}

/******************************************************************************
 * @brief Set the LowerBound.
 *
 * @param[in] e Reference to the lower bound.
 ******************************************************************************/
ADM_ALWAYS_INLINE void BranchAndBound::set_lower_bound(admission::LowerBound& e)
{
  _lbound = &e;
}

/******************************************************************************
 * @brief Set the LowerBound.
 *
 * @param[in] e Pointer to the lower bound.
 ******************************************************************************/
ADM_ALWAYS_INLINE void BranchAndBound::set_lower_bound(admission::LowerBound* e)
{
  set_lower_bound(*e);
}

/******************************************************************************
 * @brief Set the depth of task spawning in the openMP task directive.
 *
 * @param[in] d The parallel depth.
 ******************************************************************************/
ADM_ALWAYS_INLINE void BranchAndBound::set_parallel_depth(const plength_t d)
{
  _parallel_depth = d;
}

/******************************************************************************
 * @brief Return a pointer to the LowerBound.
 *
 * @returns A pointer to the lower bound.
 ******************************************************************************/
ADM_ALWAYS_INLINE const LowerBound* BranchAndBound::get_lower_bound()
{
  return _lbound;
}

// --------------- BranchAndBound - Internal solution helpers --------------- //

/******************************************************************************
 * @brief Updates the global optimum of the branch and bound
 *        with a new solution candidate.
 *
 * @param[in] s @todo.
 * @returns @todo.
 ******************************************************************************/
ADM_ALWAYS_INLINE bool BranchAndBound::update_global_opt(
    const OpSequence& s) const
{
  bool updated = false;
  if (s.cost() < _glob_opt_s.cost())
  {
    _glob_opt_s = s;
    this->_stats.add(GlobUpdate);
    updated = true;
    std::ofstream o("adm_glob_opt_s");
    s.write(o);
    o.close();
  }
  return updated;
}

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_OPTIMIZERS_IMPL_BRANCH_AND_BOUND_HPP_
