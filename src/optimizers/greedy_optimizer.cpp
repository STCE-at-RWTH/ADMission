// ******************************** Includes ******************************** //

#include "optimizers/greedy_optimizer.hpp"

#include "graph/DAG.hpp"
#include "graph/tikz.hpp"
#include "operations/find_eliminations.hpp"
#include "operations/op_sequence.hpp"
#include "elimination_algorithm.hpp"
#include "global_modes.hpp"

#include <boost/graph/adjacency_list.hpp>

#include <stddef.h>
#include <string>
#include <utility>

// **************************** Source contents ***************************** //

namespace admission
{

OpSequence GreedyOptimizer::solve(FaceDAG& g) const
{
  return greedy_solve(g);
}

OpSequence GreedyOptimizer::greedy_solve(FaceDAG& g, bool diagnostics) const
{
  auto elims = OpSequence::make_empty();
  auto new_elim = OpSequence::make_max();

  size_t n = 0;
  VertexDesc source, leaf;
  if (_diagnostics && diagnostics)
  {
    source = add_vertex(_meta_dag);
    admission::write_tikz("0.tex", g);
  }

  while ((new_elim = get_greedy_elim_on_any_graph(g)).cost() < OpSequence::max)
  {
    n++;
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

/**
 * Returns either
 * a preaccumulation that merges two vertices
 * or
 * the cheaptest elimination.
 */
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
