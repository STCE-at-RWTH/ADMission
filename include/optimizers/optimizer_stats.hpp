#ifndef OPTIMIZER_STATS_HPP
#define OPTIMIZER_STATS_HPP

// ******************************** Includes ******************************** //

#include "admission_config.hpp"

#include <iostream>
#include <stddef.h>

// **************************** Header contents ***************************** //

namespace admission
{

/**
 * \addtogroup Optimizers
 * \ @{
 */

/// Types of operations that can be logged.
enum StatT
{
  Cons = 0,
  Perf,
  Disc,
  Opti,
  Branch,
  Bound,
  GlobUpdate,
  Leaf
};

/// Struct for storing a benchmark of the optimizers operations.
struct OptimizerStats
{
 public:
  size_t data[8] = {0, 0, 0, 0, 0, 0};

  /// Reset all tracked values to 0.
  void reset()
  {
    for (size_t i = 0; i < 8; ++i)
    {
      data[i] = 0;
    }
  }

  /**\brief Critical addition, still correct,
   * if multiple threads try to increment a counter.
   *
   * @param what Tag of the value to increment
   * @param i value to add. Default value is 1
   */
  void add(const StatT& what, const ptrdiff_t& i = 1)
  {
#if ADM_STATS
    #pragma omp atomic
    data[what] += i;
#endif  // ADM_STATS
  }

  /**\brief Addition, may result in false results
   * if multiple threads try to increment at the same
   * time.
   *
   * @param what Tag of the value to increment
   * @param i value to add. Default value is 1
   */
  void non_critical_add(const StatT& what, const ptrdiff_t& i = 1)
  {
#if ADM_STATS
    data[what] += i;
#endif  // ADM_STATS
  }

  /**\brief Write the current counters into a single line
   * to provide the user with a live update of the
   * optimisation.
   *
   * @param[inout] os ostream to write to.
   */
  void write_human_readable(std::ostream& os = std::cout)
  {
#if ADM_STATS
    os << " Search space: " << data[Perf] << " Cuts: " << data[Bound]
       << " Branches : " << data[Branch] << " Leaves: " << data[Leaf] << "\r";
#endif  // ADM_STATS
  }

  /**\brief Write the counters in a human-readable
   * multi-line format.
   */
  void write(std::ostream& os = std::cout)
  {
#if ADM_STATS
    os << "Statistics:"
       << "\n\tSearch space: " << data[Perf] << "\n\tCuts: " << data[Bound]
       << "\n\tBranches: " << data[Branch] << "\n\tLeaves: " << data[Leaf]
       << std::endl;
#endif  // ADM_STATS
  }

  /**\brief Write the counters in a condensed
   * human-readable single line format.
   */
  void write_log(std::ostream& os = std::cout)
  {
#if ADM_STATS
    os << " " << data[Cons] << " " << data[Perf] << " " << data[Opti] << " "
       << data[Disc] << " " << data[Bound] << " " << data[Branch] << " "
       << data[Leaf] << " " << data[GlobUpdate];
#endif  // ADM_STATS
  }
};

/**
 * @}
 */

}  // end namespace admission

#endif  // OPTIMSIER_STATS_HPP
