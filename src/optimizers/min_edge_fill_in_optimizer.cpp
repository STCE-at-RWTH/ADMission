// ################################ INCLUDES ################################ //

#include "optimizers/min_edge_fill_in_optimizer.hpp"

#include "operations/elimination_algorithms.hpp"
#include "operations/find_eliminations.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <limits>
#include <utility>

// ############################ SOURCE CONTENTS ############################# //

namespace admission
{

/******************************************************************************
 * @brief Returns the elimination generating least fill in.
 *
 * @param[in] g Reference to the graph.
 * @returns OpSequence containing the Op.
 ******************************************************************************/
OpSequence MinEdgeFillInOptimizer::get_min_fill_in_elim_on_any_graph(
    const admission::FaceDAG& g) const
{
  auto acc_stat = boost::get(boost::vertex_acc_stat, g);
  int f_opt = std::numeric_limits<int>::max();
  auto [opt, ij] = get_mergeable_vertex_on_any_graph(g);

  // If no suited mergeable vertex was found, find the cheapest elim.
  if (opt.cost() < OpSequence::max)
  {
    f_opt = -(out_degree(ij, g) + in_degree(ij, g));
  }

  const int V = num_vertices(g);
  // #pragma omp parallel for
  for (int ij = 0; ij < V; ++ij)
  {
    // BOOST_FOREACH(auto ijk, boost::edges(g))
    BOOST_FOREACH(auto ijk, boost::out_edges(ij, g))
    {
      VertexDesc ij = source(ijk, g);
      VertexDesc jk = target(ijk, g);
      int f = 0;

      if (!in_degree(ij, g) || !out_degree(jk, g))
      {
        continue;
      }

      auto [ipkp_exists, ipkp_is_acc, iprimekprime] = check_absorption(
          ij, jk, g);
      auto [ipjp_exists, ipjp_is_acc, ipjp] = check_merge<upper_accessor>(
          ij, jk, g);
      auto [jpkp_exists, jpkp_is_acc, jpkp] = check_merge<lower_accessor>(
          jk, ij, g);

      // If ik is not absorbed we have fill in.
      if (!ipkp_is_acc)
      {
        f += out_degree(jk, g) + in_degree(ij, g);
      }
      // if ij becomes isolated, its in-edges are removed.
      if (out_degree(ij, g) == 1 || (acc_stat[ij] && ipjp_is_acc))
      {
        f -= in_degree(ij, g) + out_degree(ij, g) - 1;
      }
      // if jk becomes isolated, its out-edges are removed.
      if (in_degree(jk, g) == 1 || (acc_stat[jk] && jpkp_is_acc))
      {
        f -= out_degree(jk, g) + in_degree(jk, g) - 1;
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
            s += cheapest_preacc(iprimekprime, g);
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
