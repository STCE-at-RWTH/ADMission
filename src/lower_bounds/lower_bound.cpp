// ################################ INCLUDES ################################ //

#include "lower_bounds/lower_bound.hpp"

// ############################ SOURCE CONTENTS ############################# //

namespace admission
{

/******************************************************************************
 * @brief Applies the lower bound to a face DAG.
 *
 * Just returns 0 in case of the base class and
 * is overwritten by derived classes.
 *
 * @param g Reference to the DAG we compute the bound on.
 * @returns 0 (which is a valid lower bound for JA on any face DAG).
 ******************************************************************************/
flop_t LowerBound::operator()(const FaceDAG&) const
{
  return 0;
}

}  // end namespace admission

// ################################## EOF ################################### //
