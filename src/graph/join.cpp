// ******************************** Includes ******************************** //

#include "graph/join.hpp"

#include "graph/neighbors.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/pending/property.hpp>

#include <assert.h>
#include <iostream>

// **************************** Source contents ***************************** //

namespace admission
{

/**Joins vertices of a DAG that have identical predecessor
 * and successor sets. For matrix-free vector DAGs, the
 * solution on the simplified (joined) DAG is not equal
 * to the cost on the original DAG.
 * It holds: c_opt(G) <= c_opt(G_joined).
 */
void join_vertices(DAG& g)
{
  using boost::get;
  auto size = get(boost::vertex_size, g);

  auto Fprime_exists = get(boost::edge_acc_stat, g);
  auto Fbardot_exists = get(boost::edge_has_model, g);
  auto c_tan = get(boost::edge_c_tan, g);
  auto c_adj = get(boost::edge_c_adj, g);

  BOOST_FOREACH(auto j, vertices(g))
  {
    if (in_degree(j, g) || out_degree(j, g))
    {
      BOOST_FOREACH(auto k, vertices(g))
      {
        if (j != k)
        {
          bool s = in_degree(j, g) == in_degree(k, g) &&
                   out_degree(j, g) == out_degree(k, g);
          std::cout << "init s=" << s << std::endl;

          if (s && in_degree(j, g))
          {
            s = s && same_neighbors<DAG, lower_accessor>(g, j, k);
            std::cout << "same succ s=" << s << std::endl;
            if (s)
            {
              BOOST_FOREACH(auto ij, in_edges(j, g))
              {
                auto [ik, was_inserted] = add_edge(source(ij, g), k, g);
                assert(was_inserted == false);
                s = s && Fprime_exists[ij] == Fprime_exists[ik] &&
                    Fbardot_exists[ij] == Fbardot_exists[ik];
                std::cout << "same edge state s=" << s << std::endl;
              }
            }
          }

          if (s && out_degree(j, g))
          {
            s = s && same_neighbors<DAG, upper_accessor>(g, j, k);
            std::cout << "same pred s=" << s << std::endl;
            if (s)
            {
              BOOST_FOREACH(auto jl, out_edges(j, g))
              {
                auto [kl, was_inserted] = add_edge(k, target(jl, g), g);
                assert(was_inserted == false);
                s = s && Fprime_exists[jl] == Fprime_exists[kl] &&
                    Fbardot_exists[jl] == Fbardot_exists[kl];
                std::cout << "same edge state s=" << s << std::endl;
              }
            }
          }

          if (s)
          {
            if (out_degree(j, g))
            {
              BOOST_FOREACH(auto jl, out_edges(j, g))
              {
                auto [kl, was_inserted] = add_edge(k, target(jl, g), g);
                assert(was_inserted == false);
                c_tan[jl] = c_tan[jl] + c_tan[kl];
                c_adj[jl] = c_adj[jl] + c_adj[kl];
              }
            }
            if (in_degree(j, g))
            {
              BOOST_FOREACH(auto ij, in_edges(j, g))
              {
                auto [ik, was_inserted] = add_edge(source(ij, g), k, g);
                assert(was_inserted == false);
                c_adj[ij] = c_adj[ij] + c_adj[ik];
                c_tan[ij] = c_tan[ij] + c_tan[ik];
              }
            }
            size[j] += size[k];
            clear_vertex(k, g);
          }
        }
      }
    }
  }
}

}  // end namespace admission
