#ifndef GREEDY_OPTIMIZER_HPP
#define GREEDY_OPTIMIZER_HPP

// ******************************** Includes ******************************** //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "operations/op_sequence.hpp"
#include "optimizers/optimizer.hpp"
#include "factory.hpp"

#include <stdexcept>

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

/**\brief GreedyOptimizer that simply calls the apply_best_elims() function
 * on g.
 *
 * Only implements the solve() function of its base class and
 * the settings functions.
 */
class GreedyOptimizer : public Optimizer
{
 public:
  /// Make the base class available.
  typedef Optimizer Base;
  /// Make the transitive base class available.
  typedef typename Base::AbstractBase AbstractBase;

  ///\name Constructor and Destructor.
  ///@{
  GreedyOptimizer() = default;

  virtual ~GreedyOptimizer() override {}

  ///@}

  ///\name Override virtual functions to query certain members.
  ///@{

  /// A GreedyOptimizer does not have a LowerBound.
  virtual bool has_lower_bound() override
  {
    return false;
  }

  /// A GreedyOptimizer can not be parallelised.
  virtual bool is_parallel() override
  {
    return false;
  }

  /// Calling set_lower_bound() throws an exception.
  virtual void set_lower_bound(admission::LowerBound&) override
  {
    throw std::runtime_error(
        "LowerBound can not be set for an GreedyOptimizer.");
  }

  /// Calling set_lower_bound() throws an exception.
  virtual void set_lower_bound(admission::LowerBound* e) override
  {
    set_lower_bound(*e);
  }

  /// Calling set_parallel_depth() throws an exception.
  virtual void set_parallel_depth(const plength_t) override
  {
    throw std::runtime_error("GreedyOptimizer is not parallel.");
  }

  /// We return a nullptr if asked for the LowerBound.
  virtual const LowerBound* get_lower_bound() override
  {
    return nullptr;
  }

  ///@}

  ///\name Main interface.
  ///@{
  /**\brief Overloaded solve-function calls itself recursively.
   *
   *  @param g reference to the graph.
   */
  virtual OpSequence solve(FaceDAG& g) const override;
  ///@}

  /** \brief Recursively get the best elimination for g,
   * and apply it.
   *
   * @param[in] g Reference to the FaceDAG.
   */
  OpSequence greedy_solve(FaceDAG&, bool = true) const;

 protected:
  ///\name Internal solution helpers.
  ///@{
  /**\brief Returns either a preaccumulation that merges
   *        two vertices or the cheaptest elimination.
   * @param g Reference to the face DAG we are searching.
   * @returns OpSequence containing a single elimination.
   */
  OpSequence get_greedy_elim_on_any_graph(const admission::FaceDAG& g) const;
  ///@}
};

ADM_REGISTER_TYPE(GreedyOptimizer, GreedyOptimizer);

/**
 * @}
 */

}  // end namespace admission

#endif  // GREEDY_OPTIMIZER_HPP
