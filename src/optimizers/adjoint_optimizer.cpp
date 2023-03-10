// ******************************** Includes ******************************** //

#include "optimizers/adjoint_optimizer.hpp"

#include "graph/utils.hpp"
#include "operations/elimination_algorithms.hpp"
#include "operations/find_eliminations.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <assert.h>
#include <utility>

// **************************** Source contents ***************************** //

namespace admission
{

// ----------------- Adjoint Optimizer - Main Interface --------------------- //

/******************************************************************************
 * @brief Find a sequence that accumulates g in adjoint mode.
 *
 * @param[in] g Reference to the FaceDAG.
 * @returns OpSequence a minimal-cost sparse adjoint sequence.
 ******************************************************************************/
OpSequence AdjointOptimizer::adjoint_solve(FaceDAG& g) const
{
  auto elims = OpSequence::make_empty();
  auto new_elim = OpSequence::make_max();

  while ((new_elim = get_adjoint_elim_on_any_graph(g)).cost() < OpSequence::max)
  {
    new_elim.apply(g);
    elims += std::move(new_elim);
  }
  elims += global_adjoint_preaccumulation_ops(g);
  preaccumulate_all(g);
  return elims;
}

/******************************************************************************
 * @brief Find a sequence that accumulates g in adjoint mode.
 *
 * @param[in] g Reference to the FaceDAG.
 * @returns OpSequence a minimal-cost sparse adjoint sequence.
 ******************************************************************************/
OpSequence AdjointOptimizer::solve(FaceDAG& g) const
{
  return adjoint_solve(g);
}

// ------------- Adjoint Optimizer - Internal solution helpers -------------- //

/******************************************************************************
 * @brief Returns the operation sequence that preaccumulates all vertices.
 *
 * @param[in] g Reference to the graph.
 * @returns OpSequence containing the preaccumulation sequence.
 ******************************************************************************/
OpSequence AdjointOptimizer::global_adjoint_preaccumulation_ops(
    const FaceDAG& g) const
{
  auto acc_stat = boost::get(boost::vertex_acc_stat, g);
  auto has_model = boost::get(boost::vertex_has_model, g);
  auto elims = OpSequence::make_empty();

  BOOST_FOREACH(auto ij, boost::vertices(g))
  {
    if (in_degree(ij, g) && out_degree(ij, g) && acc_stat[ij] == false &&
        has_model[ij] == true)
    {
      elims += adj_preacc(ij, g);
    }
  }
  return elims;
}

/******************************************************************************
 * @brief Returns the elimination generating least fill in.
 *
 * @param[in] g Reference to the graph.
 * @returns OpSequence containing the Op.
 ******************************************************************************/
OpSequence AdjointOptimizer::get_adjoint_elim_on_any_graph(
    const FaceDAG& g) const
{
  auto acc_stat = boost::get(boost::vertex_acc_stat, g);
  auto elims = OpSequence::make_empty();

  BOOST_FOREACH(auto ijk, boost::edges(g))
  {
    VertexDesc ij = source(ijk, g);
    VertexDesc jk = target(ijk, g);

    // We discard minimal and maximal vertices
    // and all that have incoming paths longer than 1.
    if (!in_degree(ij, g) || !out_degree(jk, g) || longest_path_from(g, ij) > 2)
    {
      continue;
    }

    if (acc_stat[jk] == false)
    {
      assert(boost::get(boost::vertex_has_model, g, jk));
      elims = adj_preacc(jk, g);
    }
    if (acc_stat[ij] == false)
    {
      assert(boost::get(boost::vertex_has_model, g, ij));
      elims += adj_prop(ijk, g);
    }
    else
    {
      elims += mul(ijk, g);
    }
    return elims;
  }
  return OpSequence::make_max();
}

// ################################## EOF ################################### //

}  // end namespace admission
