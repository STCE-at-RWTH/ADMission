#ifndef ADM_INC_OPERATIONS_IMPL_FIND_ELIMINATIONS_HPP_
#define ADM_INC_OPERATIONS_IMPL_FIND_ELIMINATIONS_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "operations/find_eliminations.hpp"  // IWYU pragma: keep
#include "operations/op_sequence.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <utility>

// ###################### TEMPLATE AND INLINE CONTENTS ###################### //

namespace admission
{

/******************************************************************************
 * @brief Creates an OpSequence that contains
 *        a tangent propagation through (i,j,k).
 *
 * Assembles an OpSequence by calculating the cost.
 *
 * @param ijk EdgeDesc of the edge that would be eliminated.
 * @param g const Reference to the DAG.
 * @returns ElimSeqeuence containing a single EliOp in TAN direction.
 ******************************************************************************/
ADM_ALWAYS_INLINE OpSequence tan_prop(const EdgeDesc ijk, const FaceDAG& g)
{
  auto i = *(in_edges(source(ijk, g), g).first);
  auto c_tan = get(boost::vertex_c_tan, g);
  auto n = get(boost::edge_size, g);

  return {make_eli_op(ijk, g, n[i] * c_tan[target(ijk, g)], TAN)};
}

/******************************************************************************
 * @brief Creates an OpSequence that contains
 *        an adjoint propagation through (i,j,k).
 *
 * Assembles an OpSequence by calculating the cost.
 *
 * @param ijk EdgeDesc of the edge that would be eliminated.
 * @param g const Reference to the DAG.
 * @returns ElimSeqeuence containing a single EliOp in ADJ direction.
 ******************************************************************************/
ADM_ALWAYS_INLINE OpSequence adj_prop(const EdgeDesc ijk, const FaceDAG& g)
{
  auto k = *(out_edges(target(ijk, g), g).first);
  auto c_adj = get(boost::vertex_c_adj, g);
  auto n = get(boost::edge_size, g);

  return {make_eli_op(ijk, g, n[k] * c_adj[source(ijk, g)], ADJ)};
}

/******************************************************************************
 * @brief Creates an OpSequence that contains an elimination
 *        of (i,j,k) by matrix-matrix multiplication.
 *
 * Assembles an OpSequence by calculating the cost.
 *
 * @param ijk EdgeDesc of the edge that would be eliminated.
 * @param g const Reference to the DAG.
 * @returns ElimSeqeuence containing a single EliOp with MUL.
 ******************************************************************************/
ADM_ALWAYS_INLINE OpSequence mul(const EdgeDesc ijk, const FaceDAG& g)
{
  auto n = get(boost::edge_size, g);

  auto ij = source(ijk, g);
  auto jk = target(ijk, g);

  auto i = *(in_edges(ij, g).first);
  auto k = *(out_edges(jk, g).first);

  flop_t c = n[ijk] * n[i] * n[k];

  return {make_eli_op(ijk, g, c, MUL)};
}

/******************************************************************************
 * @brief Returns the elimination Sequence that
 *        accumulates vertex v for the cheapest cost.
 *
 * @param ij VertexDesc of the vertex to be accumulated.
 * @param g  Const refrence to the DAG.
 * @returns OpSequence containing a single AccOp.
 ******************************************************************************/
ADM_ALWAYS_INLINE OpSequence
cheapest_preacc(const VertexDesc ij, const FaceDAG& g)
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

/******************************************************************************
 * @brief Returns the elimination Sequence that
 *        accumulates vertex v in tangent mode.
 *
 * @param ij VertexDesc of the vertex to be accumulated.
 * @param g  Const refrence to the DAG.
 * @returns OpSequence containing a single AccOp.
 ******************************************************************************/
ADM_ALWAYS_INLINE OpSequence tan_preacc(const VertexDesc ij, const FaceDAG& g)
{
  auto c_tan = get(boost::vertex_c_tan, g);
  auto n = get(boost::edge_size, g);
  auto i = *(in_edges(ij, g).first);

  return {make_acc_op(ij, g, n[i] * c_tan[ij], TAN)};
}

/******************************************************************************
 * @brief Returns the elimination Sequence that
 *        accumulates vertex v in adjoint mode.
 *
 * @param ij VertexDesc of the vertex to be accumulated.
 * @param g  Const refrence to the DAG.
 * @returns OpSequence containing a single AccOp.
 ******************************************************************************/
ADM_ALWAYS_INLINE OpSequence adj_preacc(const VertexDesc ij, const FaceDAG& g)
{
  auto c_adj = get(boost::vertex_c_adj, g);
  auto n = get(boost::edge_size, g);
  auto j = *(out_edges(ij, g).first);

  return {make_acc_op(ij, g, n[j] * c_adj[ij], ADJ)};
}

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_OPERATIONS_IMPL_FIND_ELIMINATIONS_HPP_
