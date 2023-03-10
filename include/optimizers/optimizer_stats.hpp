#ifndef ADM_INC_OPTIMIZERS_OPTIMIZER_STATS_HPP_
#define ADM_INC_OPTIMIZERS_OPTIMIZER_STATS_HPP_

// ################################ INCLUDES ################################ //

#include "misc/doxygen.hpp"

#include <iostream>
#include <stddef.h>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(Optimizers)

/******************************************************************************
 * @enum StatT
 * @brief Types of operations that can be logged.
 ******************************************************************************/
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

/******************************************************************************
 * @brief Struct for storing a benchmark of the optimizers operations.
 ******************************************************************************/
struct OptimizerStats
{
 public:
  //! Counters for tracked data.
  size_t data[8] = {0, 0, 0, 0, 0, 0};

  //! Reset all tracked values to 0.
  void reset();

  //! Critical addition, still correct, if multiple threads try to
  //! increment a counter.
  void add(const StatT& what, const ptrdiff_t& i = 1);

  //! Addition, may result in false results if multiple threads try to
  //! increment at the same time.
  void non_critical_add(const StatT& what, const ptrdiff_t& i = 1);

  //! Write the current counters into a single line to provide the user with
  //! a live update of the optimisation.
  void write_human_readable(std::ostream& os = std::cout);

  //! Write the counters in a human-readable multi-line format.
  void write(std::ostream& os = std::cout);

  //! Write the counters in a condensed human-readable single line format.
  void write_log(std::ostream& os = std::cout);
};

DOXYGEN_MODULE_END(Optimizers)

}  // end namespace admission

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "optimizers/impl/optimizer_stats.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_OPTIMIZERS_OPTIMIZER_STATS_HPP_
