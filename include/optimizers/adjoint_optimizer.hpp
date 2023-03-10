#ifndef ADM_INC_OPTIMIZERS_ADJOINT_OPTIMIZER_HPP_
#define ADM_INC_OPTIMIZERS_ADJOINT_OPTIMIZER_HPP_

// ################################ INCLUDES ################################ //

#include "graph/DAG.hpp"
#include "misc/doxygen.hpp"
#include "operations/op_sequence.hpp"
#include "optimizers/greedy_optimizer.hpp"
#include "optimizers/optimizer.hpp"
#include "util/factory.hpp"

#include <map>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(Optimizers)

/******************************************************************************
 * @brief AdjointOptimizer.
 ******************************************************************************/
class AdjointOptimizer : public GreedyOptimizer
{
 public:
  //! Make the base class available.
  typedef Optimizer Base;
  //! Make the transitive base class available.
  typedef typename Base::AbstractBase AbstractBase;

  //! Default constructor of the AdjointOptimizer.
  AdjointOptimizer() = default;

  //! Default destructor of the AdjointOptimizer.
  virtual ~AdjointOptimizer() = default;

  // -------------------------- Main Interface ------------------------------ //
  DOXYGEN_GROUP_BEGIN(Main interface, )

  //! Find a sequence that accumulates g in adjoint mode.
  virtual OpSequence solve(FaceDAG& g) const override;

  //! Find a sequence that accumulates g in adjoint mode.
  OpSequence adjoint_solve(FaceDAG& g) const;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //

 protected:
  // ---------------------- Internal solution helpers ----------------------- //
  DOXYGEN_GROUP_BEGIN(Internal solution helpers, )

  //! Returns the operation sequence that preaccumulates all vertices.
  OpSequence global_adjoint_preaccumulation_ops(const FaceDAG& g) const;

  //! Returns the elimination generating least fill in.
  virtual OpSequence get_adjoint_elim_on_any_graph(
      const admission::FaceDAG& g) const;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //
};

ADM_REGISTER_TYPE(SparseAdjoint, AdjointOptimizer)

DOXYGEN_MODULE_END(Optimizers)

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_OPTIMIZERS_ADJOINT_OPTIMIZER_HPP_
