// ******************************** Includes ******************************** //

#include "optimizers/tangent_optimizer.hpp"

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

// ----------------- Tangent Optimizer - Main Interface --------------------- //

/******************************************************************************
 * @brief Find a sequence that accumulates g in tangent mode.
 *
 * @param[in] g Reference to the FaceDAG.
 * @returns OpSequence a minimal-cost sparse tangent sequence.
 ******************************************************************************/
OpSequence TangentOptimizer::tangent_solve(FaceDAG& g) const
{
  auto elims = OpSequence::make_empty();
  auto new_elim = OpSequence::make_max();

  while ((new_elim = get_tangent_elim_on_any_graph(g)).cost() < OpSequence::max)
  {
    new_elim.apply(g);
    elims += std::move(new_elim);
  }
  elims += global_tangent_preaccumulation_ops(g);
  preaccumulate_all(g);
  return elims;
}

/******************************************************************************
 * @brief Find a sequence that accumulates g in tangent mode.
 *
 * @param[in] g Reference to the FaceDAG.
 * @returns OpSequence a minimal-cost sparse tangent sequence.
 ******************************************************************************/
OpSequence TangentOptimizer::solve(FaceDAG& g) const
{
  return tangent_solve(g);
}

// ------------- Tangent Optimizer - Internal solution helpers -------------- //

/******************************************************************************
 * @brief Returns the operation sequence that preaccumulates all vertices.
 *
 * @param[in] g Reference to the graph.
 * @returns OpSequence containing the preaccumulation sequence.
 ******************************************************************************/
OpSequence TangentOptimizer::global_tangent_preaccumulation_ops(
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
      elims += tan_preacc(ij, g);
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
OpSequence TangentOptimizer::get_tangent_elim_on_any_graph(
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
    if (!in_degree(ij, g) || !out_degree(jk, g) ||
        longest_reverse_path_from(g, jk) > 2)
    {
      continue;
    }

    if (acc_stat[ij] == false)
    {
      assert(boost::get(boost::vertex_has_model, g, ij));
      elims = tan_preacc(ij, g);
    }
    if (acc_stat[jk] == false)
    {
      assert(boost::get(boost::vertex_has_model, g, jk));
      elims += tan_prop(ijk, g);
    }
    else
    {
      elims += mul(ijk, g);
    }
    return elims;
  }
  return OpSequence::make_max();
}

}  // end namespace admission

// ################################## EOF ################################### //
