// ################################ INCLUDES ################################ //

#include "optimizers/greedy_optimizer.hpp"

#include "graph/DAG.hpp"
#include "graph/tikz.hpp"
#include "operations/elimination_algorithms.hpp"
#include "operations/find_eliminations.hpp"
#include "operations/global_modes.hpp"
#include "operations/op_sequence.hpp"

#include <boost/graph/adjacency_list.hpp>

#include <string>
#include <utility>

// ############################ SOURCE CONTENTS ############################# //

namespace admission
{

// ------------------- Greedy Optimizer - Main Interface -------------------- //

/******************************************************************************
 * @brief Recursively get the best elimination for g and apply it.
 *
 * @param[in] g Reference to the FaceDAG.
 * @param[in] diagnostics Whether to write diagnostic output (default: true).
 ******************************************************************************/
OpSequence GreedyOptimizer::greedy_solve(FaceDAG& g, bool diagnostics) const
{
  auto elims = OpSequence::make_empty();
  auto new_elim = OpSequence::make_max();

  VertexDesc source = 0;
  VertexDesc leaf = 0;
  if (_diagnostics && diagnostics)
  {
    source = add_vertex(_meta_dag);
    admission::write_tikz("0.tex", g);
  }

  while ((new_elim = get_greedy_elim_on_any_graph(g)).cost() < OpSequence::max)
  {
    new_elim.apply(g);
    elims += std::move(new_elim);
    if (_diagnostics && diagnostics)
    {
      leaf = add_vertex(_meta_dag);
      add_edge(source, leaf, _meta_dag);
      admission::write_tikz(std::to_string(leaf) + ".tex", g);
      source = leaf;
    }
  }

  elims += global_preaccumulation_ops(g);
  preaccumulate_all(g);

  if (_diagnostics && diagnostics)
  {
    VertexDesc leaf = add_vertex(_meta_dag);
    add_edge(source, leaf, _meta_dag);
    admission::write_tikz(std::to_string(leaf) + ".tex", g);
    source = leaf;
  }

  return elims;
}

/******************************************************************************
 * @brief Overloaded solve-function calls itself recursively.
 *
 * @param[in] g Reference to the graph.
 ******************************************************************************/
OpSequence GreedyOptimizer::solve(FaceDAG& g) const
{
  return greedy_solve(g);
}

// -------------- Greedy Optimizer - Internal solution helpers -------------- //

/******************************************************************************
 * @brief Returns either a preaccumulation that merges
 *        two vertices or the cheaptest elimination.
 * @param g Reference to the face DAG we are searching.
 * @returns OpSequence containing a single elimination.
 ******************************************************************************/
OpSequence GreedyOptimizer::get_greedy_elim_on_any_graph(
    const admission::FaceDAG& g) const
{
  // Check if there are vertices that are mergeable by preaccumulation
  // cheaper than propagating their neighboring Jacobians through them.
  auto [opt, ij] = get_mergeable_vertex_on_any_graph(g);

  // If no suited mergeable vertex was found, find the cheapest elim.
  if (opt.cost() == OpSequence::max)
  {
    opt = get_cheapest_elim_on_any_graph(g);
  }
  return opt;
}

}  // end namespace admission

// ################################## EOF ################################### //
