#ifndef ADM_INC_OPTIMIZERS_TANGENT_OPTIMIZER_HPP_
#define ADM_INC_OPTIMIZERS_TANGENT_OPTIMIZER_HPP_

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
 * @brief TangentOptimizer that executes those eliminations
 *        that generate the least fill-in edges.
 ******************************************************************************/
class TangentOptimizer : public GreedyOptimizer
{
 public:
  //! Make the base class available.
  typedef Optimizer Base;
  //! Make the transitive base class available.
  typedef typename Base::AbstractBase AbstractBase;

  //! Default constructor of the TangentOptimizer.
  TangentOptimizer() = default;

  //! Default destructor of the TangentOptimizer.
  virtual ~TangentOptimizer() = default;

  // ---------------------------- Main interface ---------------------------- //
  DOXYGEN_GROUP_BEGIN(Main interface, )

  //! Find a sequence that accumulates g in tangent mode.
  OpSequence tangent_solve(FaceDAG& g) const;

  //! Find a sequence that accumulates g in tangent mode.
  virtual OpSequence solve(FaceDAG& g) const override;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //

 protected:
  // ---------------------- Internal solution helpers ----------------------- //
  DOXYGEN_GROUP_BEGIN(Internal solution helpers, )

  //! Returns the operation sequence that preaccumulates all vertices.
  OpSequence global_tangent_preaccumulation_ops(const FaceDAG& g) const;

  //! Returns the elimination generating least fill in.
  virtual OpSequence get_tangent_elim_on_any_graph(
      const admission::FaceDAG& g) const;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //
};

ADM_REGISTER_TYPE(SparseTangent, TangentOptimizer)

DOXYGEN_MODULE_END(Optimizers)

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_OPTIMIZERS_TANGENT_OPTIMIZER_HPP_
