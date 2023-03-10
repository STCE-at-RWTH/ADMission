#ifndef ADM_INC_OPTIMIZERS_IMPL_OPTIMIZER_STATS_HPP_
#define ADM_INC_OPTIMIZERS_IMPL_OPTIMIZER_STATS_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "optimizers/optimizer_stats.hpp"  // IWYU pragma: keep

#include <iostream>
#include <stddef.h>

// ###################### TEMPLATE AND INLINE CONTENTS ###################### //

namespace admission
{

/******************************************************************************
 * @brief Reset all tracked values to 0.
 ******************************************************************************/
ADM_ALWAYS_INLINE void OptimizerStats::reset()
{
  for (size_t i = 0; i < 8; ++i)
  {
    data[i] = 0;
  }
}

/******************************************************************************
 * @brief Critical addition, still correct,
 *        if multiple threads try to increment a counter.
 *
 * @param what Tag of the value to increment
 * @param i value to add (default: 1).
 ******************************************************************************/
ADM_ALWAYS_INLINE void OptimizerStats::add(const StatT& what, const ptrdiff_t& i)
{
#if ADM_STATS
  #pragma omp atomic
  data[what] += i;
#endif  // ADM_STATS
}

/******************************************************************************
 * @brief Addition, may result in false results if multiple threads try to
 *        increment at the same time.
 *
 * @param what Tag of the value to increment
 * @param i value to add (default: 1).
 ******************************************************************************/
ADM_ALWAYS_INLINE void OptimizerStats::non_critical_add(
    const StatT& what, const ptrdiff_t& i)
{
#if ADM_STATS
  data[what] += i;
#endif  // ADM_STATS
}

/******************************************************************************
 * @brief Write the current counters into a single line to provide the user
 *        with a live update of the optimisation.
 *
 * @param[inout] os ostream to write to (default: std::cout).
 ******************************************************************************/
ADM_ALWAYS_INLINE void OptimizerStats::write_human_readable(std::ostream& os)
{
#if ADM_STATS
  os << " number of discovered branches: " << data[Perf]
     << " number of explored branches: " << data[Branch] << std::endl;
#endif  // ADM_STATS
}

/******************************************************************************
 * @brief Write the counters in a human-readable multi-line format.
 *
 * @param[inout] os ostream to write to (default: std::cout).
 ******************************************************************************/
ADM_ALWAYS_INLINE void OptimizerStats::write(std::ostream& os)
{
#if ADM_STATS
  os << "branch and bound statistics:"
     << "\n  number of discovered branches: " << data[Perf]
     << "\n  number of explored branches: " << data[Branch] << std::endl;
#endif  // ADM_STATS
}

/******************************************************************************
 * @brief Write the counters in a condensed human-readable single line format.
 *
 * @param[inout] os ostream to write to (default: std::cout).
 ******************************************************************************/
ADM_ALWAYS_INLINE void OptimizerStats::write_log(std::ostream& os)
{
#if ADM_STATS
  os << " " << data[Cons] << " " << data[Perf] << " " << data[Disc] << " "
     << data[Branch] << " "
     << " " << data[GlobUpdate];
#endif  // ADM_STATS
}

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_OPTIMIZERS_IMPL_OPTIMIZER_STATS_HPP_
