#ifndef ADM_INC_OPTIMIZERS_MIN_FILL_IN_OPTIMIZER_HPP_
#define ADM_INC_OPTIMIZERS_MIN_FILL_IN_OPTIMIZER_HPP_

// ################################ INCLUDES ################################ //

#include "graph/DAG.hpp"
#include "misc/doxygen.hpp"
#include "operations/op_sequence.hpp"
#include "optimizers/greedy_optimizer.hpp"

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(Optimizers)

/******************************************************************************
 * @brief MinFillInOptimizer that executes those eliminations
 *        that generate the least fill-in edges.
 ******************************************************************************/
class MinFillInOptimizer : public GreedyOptimizer
{
 public:
  //! Make the base class available.
  typedef GreedyOptimizer Base;
  //! Make the transitive base class available.
  typedef typename Base::AbstractBase AbstractBase;

  //! Default constructor of the MinFillInOptimizer.
  MinFillInOptimizer() = default;

  //! Default destructor of the MinFillInOptimizer.
  virtual ~MinFillInOptimizer() = default;

  // -------------------------- Main Interface ------------------------------ //
  DOXYGEN_GROUP_BEGIN(Main interface, )

  //! Recursively get the Elimination that generates
  //! the least fill-in and apply it.
  OpSequence min_fill_in_solve(FaceDAG& g, bool write = true) const;

  //! Recursively get the Elimination that generates
  //! the least fill-in and apply it.
  virtual OpSequence solve(FaceDAG& g) const override;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //

 protected:
  // ---------------------- Internal solution helpers ----------------------- //
  DOXYGEN_GROUP_BEGIN(Internal solution helpers, )

  //! Returns the elimination generating least fill in.
  virtual OpSequence get_min_fill_in_elim_on_any_graph(
      const admission::FaceDAG&) const;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //
};

// TODO: This optimizer is disabled for the first release of admission.
// ADM_REGISTER_TYPE(MinFillInOptimizer, MinFillInOptimizer)

DOXYGEN_MODULE_END(Optimizers)

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_OPTIMIZERS_MIN_FILL_IN_OPTIMIZER_HPP_
