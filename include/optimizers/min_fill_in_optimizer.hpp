#ifndef MIN_FILL_IN_OPTIMIZER_HPP
#define MIN_FILL_IN_OPTIMIZER_HPP

// ******************************** Includes ******************************** //

#include "graph/DAG.hpp"
#include "operations/op_sequence.hpp"
#include "optimizers/greedy_optimizer.hpp"
#include "factory.hpp"

// **************************** Header contents ***************************** //

namespace admission
{

/**
 * \addtogroup Optimizers
 * @{
 */

/**\brief MinFillInOptimizer that executes those eliminations
 *        that generate the least fill-in edges.
 */
class MinFillInOptimizer : public GreedyOptimizer
{
 public:
  /// Make the base class available.
  typedef GreedyOptimizer Base;
  /// Make the transitive base class available.
  typedef typename Base::AbstractBase AbstractBase;

  ///\name Constructor and Destructor.
  ///@{
  MinFillInOptimizer() = default;

  virtual ~MinFillInOptimizer() {}

  ///@}

  ///\name Main interface.
  ///@{
  /** \brief Recursively get the Elimination that generates
   *         the least fill-in and apply it.
   *
   * @param[in] g Reference to the FaceDAG.
   */
  virtual OpSequence solve(FaceDAG& g) const override
  {
    return min_fill_in_solve(g);
  }

  OpSequence min_fill_in_solve(FaceDAG& g, bool = true) const;
  ///@}

 protected:
  ///\name Internal solution helpers.
  ///@{
  /**\brief Returns the elimination generating least fill in.
         *
         * @param[in] g Reference to the graph.
         * @returns OpSequence containing the Op.
         */
  virtual OpSequence get_min_fill_in_elim_on_any_graph(
      const admission::FaceDAG&) const;
  ///@}
};

ADM_REGISTER_TYPE(MinFillInOptimizer, MinFillInOptimizer);

/**
 * @}
 */

}  // end namespace admission

#endif  // MIN_FILL_IN_OPTIMIZER_HPP
