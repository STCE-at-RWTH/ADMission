#ifndef MIN_EDGE_FILL_IN_OPTIMIZER_HPP
#define MIN_EDGE_FILL_IN_OPTIMIZER_HPP

// ******************************** Includes ******************************** //

#include "graph/DAG.hpp"
#include "operations/op_sequence.hpp"
#include "optimizers/min_fill_in_optimizer.hpp"
#include "factory.hpp"

// **************************** Header contents ***************************** //

namespace admission
{

/**
 * \addtogroup Optimizers
 * @{
 */

/**\brief MinEdgeFillInOptimizer that executes those eliminations
 *        that generate the least fill-in edges.
 */
class MinEdgeFillInOptimizer : public MinFillInOptimizer
{
 public:
  /// Make the base class available.
  typedef MinFillInOptimizer Base;
  /// Make the transitive base class available.
  typedef typename Base::AbstractBase AbstractBase;

  ///\name Constructor and Destructor.
  ///@{
  MinEdgeFillInOptimizer() = default;

  virtual ~MinEdgeFillInOptimizer() {}

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
      const admission::FaceDAG&) const override;
  ///@}
};

ADM_REGISTER_TYPE(MinEdgeFillInOptimizer, MinEdgeFillInOptimizer);

/**
 * @}
 */

}  // end namespace admission

#endif  // MIN_EDGE_FILL_IN_OPTIMIZER_HPP
