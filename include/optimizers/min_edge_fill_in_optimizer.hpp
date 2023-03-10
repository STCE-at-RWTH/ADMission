#ifndef ADM_INC_OPTIMIZERS_MIN_EDGE_FILL_IN_OPTIMIZER_HPP_
#define ADM_INC_OPTIMIZERS_MIN_EDGE_FILL_IN_OPTIMIZER_HPP_

// ################################ INCLUDES ################################ //

#include "graph/DAG.hpp"
#include "misc/doxygen.hpp"
#include "operations/op_sequence.hpp"
#include "optimizers/min_fill_in_optimizer.hpp"
#include "util/factory.hpp"

#include <map>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(Optimizers)

/******************************************************************************
 * @brief MinEdgeFillInOptimizer that executes those eliminations
 *        that generate the least fill-in edges.
 ******************************************************************************/
class MinEdgeFillInOptimizer : public MinFillInOptimizer
{
 public:
  //! Make the base class available.
  typedef MinFillInOptimizer Base;
  //! Make the transitive base class available.
  typedef typename Base::AbstractBase AbstractBase;

  //! Default constructor of the MinEdgeFillInOptimizer.
  MinEdgeFillInOptimizer() = default;

  //! Default destructor of the MinEdgeFillInOptimizer.
  virtual ~MinEdgeFillInOptimizer() = default;

 protected:
  // ---------------------- Internal solution helpers ----------------------- //
  DOXYGEN_GROUP_BEGIN(Internal solution helpers, )

  //! Returns the elimination generating least fill in.
  virtual OpSequence get_min_fill_in_elim_on_any_graph(
      const admission::FaceDAG&) const override final;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //
};

ADM_REGISTER_TYPE(GreedyMinFill, MinEdgeFillInOptimizer)

DOXYGEN_MODULE_END(Optimizers)

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_OPTIMIZERS_MIN_EDGE_FILL_IN_OPTIMIZER_HPP_
