#ifndef ADM_INC_OPTIMIZERS_IMPL_OPTIMIZER_HPP_
#define ADM_INC_OPTIMIZERS_IMPL_OPTIMIZER_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "optimizers/optimizer.hpp"  // IWYU pragma: keep
#include "optimizers/optimizer_stats.hpp"

#include <iostream>

// ###################### TEMPLATE AND INLINE CONTENTS ###################### //

namespace admission
{

// --------------------- Optimizer - Optimizer settings --------------------- //

/******************************************************************************
 * @brief Turn on tracking of the search space and writing intermediate
 *        solutions.
 *
 * @param[in] on bool Diagnostics are on if true.
 ******************************************************************************/
ADM_ALWAYS_INLINE void Optimizer::set_diagnostics(bool on)
{
  _diagnostics = on;
}

/******************************************************************************
 * @brief Set the interval of writing single-line output.
 *
 * @param[in] t double time interval in seconds.
 ******************************************************************************/
ADM_ALWAYS_INLINE void Optimizer::set_output_interval(double t)
{
  _interval = t;
}

/******************************************************************************
 * @brief Set the mode of the output to human readable or shorter.
 *
 * @param[in] hr bool Output is human readable if true.
 ******************************************************************************/
ADM_ALWAYS_INLINE void Optimizer::set_output_mode(bool hr)
{
  _output_mode = hr;
}

/******************************************************************************
 * @brief Reset the internal elimination and branch/cut counters.
 ******************************************************************************/
ADM_ALWAYS_INLINE void Optimizer::reset()
{
  _stats.reset();
}

// ---------------------------- Optimizer - I/O ----------------------------- //

/******************************************************************************
 * @brief Prints the optimizer's stats.
 *
 * @param os std::ostream& the stream to write to.
 ******************************************************************************/
ADM_ALWAYS_INLINE void Optimizer::write(std::ostream& os)
{
  _stats.write(os);
}

/******************************************************************************
 * @brief Prints the opsimiser's stats in a less verbose form.
 *
 * @param os std::ostream& the stream to write to.
 ******************************************************************************/
ADM_ALWAYS_INLINE void Optimizer::write_log(std::ostream& os)
{
  _stats.write_log(os);
}

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_OPTIMIZERS_OPTIMIZER_HPP_
