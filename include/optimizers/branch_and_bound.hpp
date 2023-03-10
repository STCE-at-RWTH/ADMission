#ifndef ADM_INC_OPTIMIZERS_BRANCH_AND_BOUND_HPP_
#define ADM_INC_OPTIMIZERS_BRANCH_AND_BOUND_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "misc/doxygen.hpp"
#include "operations/op_sequence.hpp"
#include "optimizers/adjoint_optimizer.hpp"
#include "optimizers/estimator.hpp"
#include "optimizers/greedy_optimizer.hpp"
#include "optimizers/min_fill_in_optimizer.hpp"
#include "optimizers/optimizer.hpp"
#include "optimizers/tangent_optimizer.hpp"
#include "util/factory.hpp"

#include <map>

// ########################## FORWARD DECLARATIONS ########################## //

namespace admission
{
class LowerBound;
}

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(Optimizers)

/******************************************************************************
 * @brief Branch and bound algorithm.
 *
 * Has a reference to a class that computes a lower bound.
 * And a reference to a class that removes eliminations from
 * a set of possible eliminations.
 ******************************************************************************/
class BranchAndBound : public Optimizer
{
 public:
  //! Make the base class available.
  typedef Optimizer Base;
  //! Make the transitive base class available.
  typedef typename Base::AbstractBase AbstractBase;

  //! Default contructor of BranchAndBound.
  BranchAndBound();

  //! Contructor of BranchAndBound with a given lower bound.
  BranchAndBound(const LowerBound& lbound);

  //! Default destructor of BranchAndBound.
  virtual ~BranchAndBound() override = default;

  // -------------------------- Optimizer settings -------------------------- //
  DOXYGEN_GROUP_BEGIN(Optimizer settings, )

  //! Reset the optimal soluton cost and the counters.
  virtual void reset() override final;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //

  // ------------- Override functions to query certain members -------------- //
  DOXYGEN_GROUP_BEGIN(Override functions to query certain members, )

  //! BranchAndBound has a LowerBound.
  virtual bool has_lower_bound() override final;

  //! BranchAndBound has a LowerBound.
  virtual bool is_parallel() override final;

  //! Set the LowerBound.
  virtual void set_lower_bound(admission::LowerBound& e) override final;

  //! Set the LowerBound.
  virtual void set_lower_bound(admission::LowerBound* e) override final;

  //! Set the depth of task spawning in the openMP task directive.
  virtual void set_parallel_depth(const plength_t d) override final;

  //! Return a pointer to the LowerBound.
  virtual const LowerBound* get_lower_bound() override final;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //

  // ---------------------------- Main interface ---------------------------- //
  DOXYGEN_GROUP_BEGIN(Main interface, )

  //! Our own solve function, calling itself recursively.
  OpSequence branch_and_bound_solve(
      FaceDAG& g, const OpSequence solution_until_now,
      const flop_t parallel_depth, const VertexDesc source) const;

  //! Override the virtual solve function.
  virtual OpSequence solve(FaceDAG& g) const override final;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //

 protected:
  // ---------------------- Internal solution helpers ----------------------- //
  DOXYGEN_GROUP_BEGIN(Internal solution helpers, )

  //! Traverses all possible preaccumulations and eliminations (operations)
  //! on a face DAG. For each operation, it performs some task specified
  //! by the callables passed to it.
  template<typename V_ACTION, typename E_ACTION>
  void traverse_elims(const FaceDAG&, V_ACTION&, E_ACTION&) const;

  //! Updates the global optimum of the branch and bound
  //! with a new solution candidate.
  bool update_global_opt(const OpSequence& s) const;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //

  //! To find tangent-mode based solutions.
  admission::TangentOptimizer _tangent_optimizer;

  //! To find adjoint-mode based solutions.
  admission::AdjointOptimizer _adjoint_optimizer;

  //! To find greedy solutions.
  admission::GreedyOptimizer _greedy_optimizer;

  //! To find min-fill-in solutions.
  admission::MinFillInOptimizer _min_fill_in_optimizer;

  //! Estimate the search space size.
  mutable admission::Estimator _est;

  //! Pointer to the LowerBound.
  const admission::LowerBound* _lbound;

  //! The maximum task-spawning depth.
  plength_t _parallel_depth;

  //! Global optimal sequence allowing to compare results bewteen threads.
  mutable admission::OpSequence _glob_opt_s;
};

ADM_REGISTER_TYPE(BranchAndBound, BranchAndBound)

DOXYGEN_MODULE_END(Optimizers)

}  // end namespace admission

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "optimizers/impl/branch_and_bound.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_OPTIMIZERS_BRANCH_AND_BOUND_HPP_
