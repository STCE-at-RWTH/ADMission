#ifndef BRANCH_AND_BOUND_HPP
#define BRANCH_AND_BOUND_HPP

// ******************************** Includes ******************************** //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "operations/op_sequence.hpp"
#include "optimizers/estimator.hpp"
#include "optimizers/greedy_optimizer.hpp"
#include "optimizers/min_fill_in_optimizer.hpp"
#include "optimizers/optimizer.hpp"
#include "optimizers/optimizer_stats.hpp"
#include "factory.hpp"

#include <iosfwd>

// ************************** Forward declarations ************************** //

namespace admission
{
class LowerBound;
}

// **************************** Header contents ***************************** //

namespace admission
{

/**
 * \addtogroup Optimizers
 * @{
 */

/**\brief Branch and bound algorithm.
 *
 * Has a reference to a class that computes a lower bound.
 * And a reference to a class that removes eliminations from
 * a set of possible eliminations.
 */
class BranchAndBound : public Optimizer
{
 public:
  /// Make the base class available.
  typedef Optimizer Base;
  /// Make the transitive base class available.
  typedef typename Base::AbstractBase AbstractBase;

  ///\name Constructors & Destructors.
  ///@{
  BranchAndBound(const LowerBound& lbound)
      : _lbound(&lbound),
        _parallel_depth(1),
        _glob_opt_s(OpSequence::make_max())
  {}

  BranchAndBound()
      : _lbound(nullptr),
        _parallel_depth(1),
        _glob_opt_s(OpSequence::make_max())
  {}

  virtual ~BranchAndBound() override {}

  ///@}

  /// Reset the optimal soluton cost and the counters.
  virtual void reset() override
  {
    _glob_opt_s = OpSequence::make_max();
    Base::reset();
  }

  /// BranchAndBound has a LowerBound.
  virtual bool has_lower_bound() override final
  {
    return true;
  }

  /// BranchAndBound has a LowerBound.
  virtual bool is_parallel() override final
  {
    return true;
  }

  /// Set the LowerBound.
  virtual void set_lower_bound(admission::LowerBound& e) override final
  {
    _lbound = &e;
  }

  /// Set the LowerBound.
  virtual void set_lower_bound(admission::LowerBound* e) override final
  {
    set_lower_bound(*e);
  }

  /// Set the depth of task spawning in the openMP task directive.
  virtual void set_parallel_depth(const plength_t d) override final
  {
    _parallel_depth = d;
  }

  /// Return a pointer to the LowerBound.
  virtual const LowerBound* get_lower_bound() override final
  {
    return _lbound;
  }

  /**\brief Override the virtual solve function.
   *
   * @param[in] g FaceDAG& The input.
   * @returns OpSequence The optimal sequence.
   */
  virtual OpSequence solve(FaceDAG& g) const override;

  /**\brief Our own solve function, calling itself recursively.
   *
   * @param[in] g FaceDag& The input.
   * @param[in] cost_until_now flop_t Reference for comparison of global optima.
   * @param[in] parallel_depth flop_t Know the depth of recursion to switch openMP task parallelisation.
   * @param[in] source VertexDesc Current Vertex in the MetaDAG.
   */
  OpSequence solve(
      FaceDAG& g, const OpSequence solution_until_now,
      const flop_t parallel_depth, const VertexDesc source) const;

 protected:
  /**\brief Traverses all operations on a face DAG and executes
   *  callables on them. */
  template<typename V_ACTION, typename E_ACTION>
  void traverse_elims(const FaceDAG&, V_ACTION&, E_ACTION&) const;

  /**\brief Updates the global optimum of the branch and bound
   * with a new solution candidate. */
  ADM_ALWAYS_INLINE
  bool update_global_opt(const OpSequence& s) const
  {
    bool updated = false;
    if (s.cost() < _glob_opt_s.cost())
    {
      _glob_opt_s = s;
      this->_stats.add(GlobUpdate);
      updated = true;
      std::ofstream o("adm_glob_opt_s");
      s.write(o);
      o.close();
    }
    return updated;
  }

  /// To find greedy solutions.
  admission::GreedyOptimizer _greedy_optimizer;

  /// To find min-fill-in solutions.
  admission::MinFillInOptimizer _min_fill_in_optimizer;

  /// Estimate the search space size.
  mutable admission::Estimator _est;

  /// Pointer to the LowerBound.
  const admission::LowerBound* _lbound;

  /// The maximum task-spawning depth.
  plength_t _parallel_depth;

  /// Global optimal sequence allowing to compare results bewteen threads.
  mutable admission::OpSequence _glob_opt_s;
};

ADM_REGISTER_TYPE(BranchAndBound, BranchAndBound);

/**
 * @}
 */

}  // end namespace admission

#endif  // BRANCH_AND_BOUND_HPP
