#ifndef ADM_INC_OPTIMIZERS_GREEDY_OPTIMIZER_HPP_
#define ADM_INC_OPTIMIZERS_GREEDY_OPTIMIZER_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "misc/doxygen.hpp"
#include "operations/op_sequence.hpp"
#include "optimizers/optimizer.hpp"

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
 * @brief GreedyOptimizer that simply calls the apply_best_elims() function
 *        on g.
 *
 * Only implements the solve() function of its base class and
 * the settings functions.
 ******************************************************************************/
class GreedyOptimizer : public Optimizer
{
 public:
  //! Make the base class available.
  typedef Optimizer Base;
  //! Make the transitive base class available.
  typedef typename Base::AbstractBase AbstractBase;

  //! Default constructor of the GreedyOptimizer.
  GreedyOptimizer() = default;

  //! Default destructor of the GreedyOptimizer.
  virtual ~GreedyOptimizer() override = default;

  // ------------- Override functions to query certain members -------------- //
  DOXYGEN_GROUP_BEGIN(Override functions to query certain members, )

  //! A GreedyOptimizer does not have a LowerBound.
  virtual bool has_lower_bound() override;

  //! A GreedyOptimizer can not be parallelised.
  virtual bool is_parallel() override;

  //! Calling set_lower_bound() throws an exception.
  virtual void set_lower_bound(admission::LowerBound&) override;

  //! Calling set_lower_bound() throws an exception.
  virtual void set_lower_bound(admission::LowerBound* e) override;

  //! Calling set_parallel_depth() throws an exception.
  virtual void set_parallel_depth(const plength_t) override;

  //! We return a nullptr if asked for the LowerBound.
  virtual const LowerBound* get_lower_bound() override;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //

  // ---------------------------- Main interface ---------------------------- //
  DOXYGEN_GROUP_BEGIN(Main interface, )

  //! Recursively get the best elimination for g and apply it.
  OpSequence greedy_solve(FaceDAG&, bool diagnostics = true) const;

  //! Overloaded solve-function calls itself recursively.
  virtual OpSequence solve(FaceDAG& g) const override;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //

 protected:
  // ---------------------- Internal solution helpers ----------------------- //
  DOXYGEN_GROUP_BEGIN(Internal solution helpers, )

  //! Returns either a preaccumulation that merges
  //! two vertices or the cheaptest elimination.
  OpSequence get_greedy_elim_on_any_graph(const admission::FaceDAG& g) const;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //
};

// TODO: This optimizer is disabled for the first release of admission.
// ADM_REGISTER_TYPE(GreedyOptimizer, GreedyOptimizer)

DOXYGEN_MODULE_END(Optimizers)

}  // end namespace admission

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "optimizers/impl/greedy_optimizer.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_OPTIMIZERS_GREEDY_OPTIMIZER_HPP_
