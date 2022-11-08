#ifndef FIND_ELIMINATIONS_HPP
#define FIND_ELIMINATIONS_HPP

// ******************************** Includes ******************************** //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "operations/op_sequence.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <utility>

// **************************** Header contents ***************************** //

namespace admission
{

/**
 * \defgroup EliminationAlgorithm Elimination, Merge and Preaccumulation algorithms and helper functions.
 * \addtogroup EliminationAlgorithm
 * @{
 */

/**\brief Checks if an unaccumulated vertex has an
 *        accumulated merge candidate for a forced merge.
 *
 * This is not used by eliminate_edge()
 * but may be used by optimisation algorithms.
 * \code
 *      kl
 *      | \
 *      |  \
 *      ik F_ik
 *      |  /
 *      | /
 *      hi
 * \endcode
 * @param[in] ij Vertex for which we look for a candidate.
 * @param[in] g  The DAG.
 * @returns A std::pair containing a bool indicating if a candidate vertex
 *          was found and the vertex descriptor of the candidate vertex.
 *          Otherwise the vertex descriptor is default initialised.
 */
// ADM_ALWAYS_INLINE
std::pair<bool, VertexDesc> has_merge_candidate(
    const VertexDesc ij, const FaceDAG& g);

/**\brief Creates an OpSequence that contains
 *        a tangent propagation through (i,j,k).
 *
 * Assembles an OpSequence by calculating the cost.
 * @param ijk EdgeDesc of the edge that would be eliminated.
 * @param g const Reference to the DAG.
 * @returns ElimSeqeuence containing a single EliOp in TAN direction.
 */
ADM_ALWAYS_INLINE
OpSequence tan_prop(const EdgeDesc ijk, const FaceDAG& g)
{
  auto i = *(in_edges(source(ijk, g), g).first);
  auto c_tan = get(boost::vertex_c_tan, g);
  auto n = get(boost::edge_size, g);

  return {make_eli_op(ijk, g, n[i] * c_tan[target(ijk, g)], TAN)};
}

/**\brief Creates an OpSequence that contains
 *        an adjoint propagation through (i,j,k).
 *
 * Assembles an OpSequence by calculating the cost.
 * @param ijk EdgeDesc of the edge that would be eliminated.
 * @param g const Reference to the DAG.
 * @returns ElimSeqeuence containing a single EliOp in ADJ direction.
 */
ADM_ALWAYS_INLINE
OpSequence adj_prop(const EdgeDesc ijk, const FaceDAG& g)
{
  auto k = *(out_edges(target(ijk, g), g).first);
  auto c_adj = get(boost::vertex_c_adj, g);
  auto n = get(boost::edge_size, g);

  return {make_eli_op(ijk, g, n[k] * c_adj[source(ijk, g)], ADJ)};
}

/**\brief Creates an OpSequence that contains an elimination
 *        of (i,j,k) by matrix-matrix multiplication.
 *
 * Assembles an OpSequence by calculating the cost.
 * @param ijk EdgeDesc of the edge that would be eliminated.
 * @param g const Reference to the DAG.
 * @returns ElimSeqeuence containing a single EliOp with MUL.
 */
ADM_ALWAYS_INLINE
OpSequence mul(const EdgeDesc ijk, const FaceDAG& g)
{
  auto n = get(boost::edge_size, g);

  auto ij = source(ijk, g);
  auto jk = target(ijk, g);

  auto i = *(in_edges(ij, g).first);
  auto k = *(out_edges(jk, g).first);

  flop_t c = n[ijk] * n[i] * n[k];

  return {make_eli_op(ijk, g, c, MUL)};
}

/**\brief Returns the elimination Sequence that
 *        accumulates vertex v for the cheapest cost.
 *
 * @param ij VertexDesc of the vertex to be accumulated.
 * @param g  Const refrence to the DAG.
 * @returns OpSequence containing a single AccOp.
 */
ADM_ALWAYS_INLINE
OpSequence cheapest_preacc(const VertexDesc ij, const FaceDAG& g)
{
  auto c_tan = get(boost::vertex_c_tan, g);
  auto c_adj = get(boost::vertex_c_adj, g);
  auto n = get(boost::edge_size, g);

  flop_t acc_c;
  dir_t dir;

  auto i = *(in_edges(ij, g).first);
  auto j = *(out_edges(ij, g).first);

  if (n[i] * c_tan[ij] < n[j] * c_adj[ij])
  {
    acc_c = n[i] * c_tan[ij];
    dir = TAN;
  }
  else
  {
    acc_c = n[j] * c_adj[ij];
    dir = ADJ;
  }
  return {make_acc_op(ij, g, acc_c, dir)};
}

std::pair<OpSequence, VertexDesc> get_mergeable_vertex_on_any_graph(
    const admission::FaceDAG& g);

/**\brief Finds the cheapest way to eliminate an edge \f$(i,j,k)\f$.
 */
OpSequence get_cheapest_elim(const EdgeDesc ijk, const FaceDAG& g);

/**\brief Finds the cheapest elimination on a graph.
 */
OpSequence get_cheapest_elim_on_any_graph(const FaceDAG& g);

/**
 * @}
 */

}  // end namespace admission

#endif  // FIND_ELIMINATIONS_HPP
