// ################################ INCLUDES ################################ //

#include "optimizers/min_fill_in_optimizer.hpp"

#include "graph/tikz.hpp"
#include "operations/elimination_algorithms.hpp"
#include "operations/find_eliminations.hpp"
#include "operations/global_modes.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <limits>
#include <string>
#include <utility>

// ############################ SOURCE CONTENTS ############################# //

namespace admission
{

// ----------------- Adjoint Optimizer - Main Interface --------------------- //

/******************************************************************************
 * @brief Recursively get the Elimination that generates
 *        the least fill-in and apply it.
 *
 * @param[in] g Reference to the FaceDAG.
 * @param[in] diagnostics Whether to write diagnostic output (default: true).
 ******************************************************************************/
OpSequence MinFillInOptimizer::min_fill_in_solve(
    FaceDAG& g, bool diagnostics) const
{
  auto elims = OpSequence::make_empty();
  auto new_elim = OpSequence::make_max();

  VertexDesc source = 0;
  if (diagnostics)
  {
    source = add_vertex(_meta_dag);
    if (_diagnostics)
    {
      admission::write_tikz("0.tex", g);
    }
  }
  while ((new_elim = get_min_fill_in_elim_on_any_graph(g)).cost() <
         OpSequence::max)
  {
    new_elim.apply(g);
    elims += std::move(new_elim);
    if (_diagnostics)
    {
      VertexDesc leaf = add_vertex(_meta_dag);
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
 * @brief Recursively get the Elimination that generates
 *        the least fill-in and apply it.
 *
 * @param[in] g Reference to the FaceDAG.
 ******************************************************************************/
OpSequence MinFillInOptimizer::solve(FaceDAG& g) const
{
  return min_fill_in_solve(g);
}

/******************************************************************************
 * @brief Returns the elimination generating least fill in.
 *
 * @param[in] g Reference to the graph.
 * @returns OpSequence containing the Op.
 ******************************************************************************/
OpSequence MinFillInOptimizer::get_min_fill_in_elim_on_any_graph(
    const admission::FaceDAG& g) const
{
  auto acc_stat = boost::get(boost::vertex_acc_stat, g);
  int f_opt = std::numeric_limits<int>::max();
  auto [opt, ij] = get_mergeable_vertex_on_any_graph(g);

  // If no suited mergeable vertex was found, find the cheapest elim.
  if (opt.cost() < OpSequence::max)
  {
    f_opt = -1;  //(out_degree(ij, g) + in_degree(ij, g));
  }

  const int V = num_vertices(g);
  // #pragma omp parallel for
  for (int ij = 0; ij < V; ++ij)
  {
    BOOST_FOREACH(auto ijk, boost::out_edges(ij, g))
    {
      VertexDesc ij = source(ijk, g);
      VertexDesc jk = target(ijk, g);
      int f = 0;

      if (!in_degree(ij, g) || !out_degree(jk, g))
      {
        continue;
      }

      auto [ipkp_exists, ipkp_is_acc, ipkp] = check_absorption(ij, jk, g);
      auto [ipjp_exists, ipjp_is_acc, ipjp] = check_merge<upper_accessor>(
          ij, jk, g);
      auto [jpkp_exists, jpkp_is_acc, jpkp] = check_merge<lower_accessor>(
          jk, ij, g);

      // If ik is not absorbed we have fill in.
      if (!ipkp_is_acc)
      {
        f += 1;
      }
      // if ij becomes isolated, its in-edges are removed.
      if (out_degree(ij, g) == 1 || (acc_stat[ij] && ipjp_is_acc))
      {
        f -= 1;
      }
      // if jk becomes isolated, its out-edges are removed.
      if (in_degree(jk, g) == 1 || (acc_stat[jk] && jpkp_is_acc))
      {
        f -= 1;
      }

      OpSequence s = get_cheapest_elim(ijk, g);

      #pragma omp critical
      {
        if (f < f_opt)
        {
          opt = s;
          f_opt = f;
        }
        else if (f == f_opt)
        {
          if (ipkp_exists && !ipkp_is_acc)
          {
            // Append preaccumulation of absorbing vertex.
            s += cheapest_preacc(ipkp, g);
            f -= out_degree(jk, g) + in_degree(ij, g);
            if (s.cost() < opt.cost())
            {
              opt = s;
              f_opt = f;
            }
          }
        }
      }
    }
  }

  return opt;
}

}  // end namespace admission

// ################################## EOF ################################### //
