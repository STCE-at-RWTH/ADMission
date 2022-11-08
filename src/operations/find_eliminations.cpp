// ******************************** Includes ******************************** //

#include "operations/find_eliminations.hpp"

#include "graph/neighbors.hpp"

#include <boost/foreach.hpp>

#include <type_traits>

// **************************** Source contents ***************************** //

namespace admission
{

/**
 * \defgroup EliminationAlgorithm Elimination, Merge and Preaccumulation algorithms and helper functions.
 * \addtogroup EliminationAlgorithm
 * @{
 */

std::pair<bool, VertexDesc> has_merge_candidate(
    const VertexDesc ij, const FaceDAG& g)
{
  auto Jprime_exists = get(boost::vertex_acc_stat, g);
  if (Jprime_exists[ij] == true)
  {
    return std::make_pair(false, VertexDesc());
  }

  // Iterate over all successors (jk) of (ij) ...
  BOOST_FOREACH(auto oe, out_edges(ij, g))
  {
    VertexDesc jk = target(oe, g);

    // ... and over all predecessors (hj) of all successors (jk)
    BOOST_FOREACH(auto ie, in_edges(jk, g))
    {
      VertexDesc hj = source(ie, g);
      if (hj == ij)
      {
        continue;
      }

      // Check if succ. set and pred. sets are identical
      // (1.) Condition for generation of fill-in.
      // P(i,*,k) = P(i,*,j) && S(i,*,k) = S(j,*,k) && F'(j,*,k) != \emptyset
      if (same_predecessors(g, hj, ij) && same_successors(g, hj, ij) &&
          Jprime_exists[hj])
      {
        return std::make_pair(true, hj);
      }
    }
  }
  return std::make_pair(false, VertexDesc());
}

std::pair<OpSequence, VertexDesc> get_mergeable_vertex_on_any_graph(
    const admission::FaceDAG& g)
{
  std::pair<OpSequence, VertexDesc> res(OpSequence::make_max(), VertexDesc());
  auto n = boost::get(boost::edge_size, g);

  // Check if there are vertices that are mergeable by preaccumulation
  // cheaper than propagating their neighboring Jacobians through them.
  const int V = num_vertices(g);

  #pragma omp parallel for
  for (int ij = 0; ij < V; ++ij)
  {
    auto&& [has_candidate, iprimejprime] = has_merge_candidate(ij, g);
    if (has_candidate == true)
    {
      auto i = *(in_edges(ij, g).first);
      auto j = *(out_edges(ij, g).first);

      // Find the cheapest preaccumulation...
      auto acc_s = cheapest_preacc(ij, g);

      // ...and if its in TAN direction check the predecessors.
      if ((acc_s.front())._dir == TAN)
      {
        flop_t in_sum = 0;
        BOOST_FOREACH(auto e, in_edges(ij, g))
        {
          if (in_degree(source(e, g), g))
          {
            in_sum += n[*(in_edges(source(e, g), g).first)];
          }
        }
        // If we found a suited preaccumulation, we are done.
        if (n[i] < in_sum)
        {
          res = std::make_pair(acc_s, ij);
        }
      }
      // ... if its in ADJ direction check the succcessors.
      else
      {
        flop_t out_sum = 0;
        BOOST_FOREACH(auto e, out_edges(ij, g))
        {
          if (out_degree(target(e, g), g))
          {
            out_sum += n[*(out_edges(target(e, g), g).first)];
          }
        }
        // If we found a suited preaccumulation, we are done.
        if (n[j] < out_sum)
        {
          res = std::make_pair(acc_s, ij);
        }
      }
    }
  }
  return res;
}

OpSequence get_cheapest_elim(const EdgeDesc ijk, const FaceDAG& g)
{
  VertexDesc ij = source(ijk, g), jk = target(ijk, g);

  auto acc_stat = boost::get(boost::vertex_acc_stat, g);
  auto hm = boost::get(boost::vertex_has_model, g);

  OpSequence opt = OpSequence::make_max();

  // A Lambda for simplifying the update of the optimum.
  auto update = [&](const OpSequence& new_elim)
  {
    if (new_elim.cost() < opt.cost())
    {
      opt = new_elim;
    }
  };

  // Populate candidate sequences.
  OpSequence tan_s = tan_prop(ijk, g);
  OpSequence adj_s = adj_prop(ijk, g);
  OpSequence mul_s = mul(ijk, g);
  OpSequence ij_acc = cheapest_preacc(ij, g);
  OpSequence jk_acc = cheapest_preacc(jk, g);

  // Case 1: Both jacobians are accumulated.
  // =======================================
  // We can
  //  - propagate \f$F'_{ij}\f$ through \f$\dot F_{jk}\f$ if (jk) has a tangent model
  //  - propagate \f$F'_{jk}\f$ through \f$\bar F_{ij}\f$ if (ij) has an adjoint model
  //  - multiply \f$F'_{jk} F'_{ij}\f$
  if ((acc_stat[ij] == true) && (acc_stat[jk] == true))
  {
    if (hm[jk])
    {
      update(tan_s);
    }
    if (hm[ij])
    {
      update(adj_s);
    }
    update(mul_s);
  }
  // Case 2: ij is accumulated, jk is not.
  // =====================================
  // We can
  //  - accumulate \f$F'_{jk}\f$ and multiply \f$F'_{jk} F'_{ij}\f$
  //  - if (ij) has an ADJ model
  //    - accumulate \f$F'_{jk}\f$ and propagate through \f$\bar F_{ij}\f$ in ADJ mode
  //  - propagate \f$F'_{ij}\f$ through \f$\dot F_{jk}\f$ in TAN mode.
  else if ((acc_stat[ij] == true) && (acc_stat[jk] == false))
  {
    update(jk_acc + mul_s);
    if (hm[ij])
    {
      update(jk_acc + adj_s);
    }
    update(tan_s);
  }
  // Case 3: jk is accumulated, ij is not.
  // =====================================
  // We can
  //  - accumulate \f$F'_{ij}\f$ and multiply \f$F'_{jk} F'_{ij}\f$
  //  - if (jk) has a TAN model
  //    - accumulate \f$F'_{ij}\f$ and propagate through \f$\dot F_{jk}\f$ in TAN mode
  //  - propagate \f$F'_{jk}\f$ through \f$\bar F_{ij}\f$ in ADJ mode.
  else if ((acc_stat[ij] == false) && (acc_stat[jk] == true))
  {
    update(ij_acc + mul_s);
    if (hm[jk])
    {
      update(ij_acc + tan_s);
    }
    update(adj_s);
  }
  // Case 4: ij and jk are not accumulated.
  // ======================================
  // We can
  //  - accumulate \f$F'_{ij}\f$ and \f$F'_{jk}\f$ and multiply \f$F'_{jk} F'_{ij}\f$
  //  - accumulate \f$F'_{ij}\f$ and propagate through \f$\dot F_{jk}\f$ in TAN mode
  //  - accumulate \f$F'_{jk}\f$ and propagate through \f$\bar F_{ij}\f$ in ADJ mode
  else
  {
    update(ij_acc + jk_acc + mul_s);
    update(ij_acc + tan_s);
    update(jk_acc + adj_s);
  }
  return opt;
}

OpSequence get_cheapest_elim_on_any_graph(const FaceDAG& g)
{
  OpSequence opt = OpSequence::make_max();
  const int V = num_vertices(g);

  #pragma omp parallel for
  for (int ij = 0; ij < V; ++ij)
  {
    // BOOST_FOREACH(auto ijk, boost::edges(g))
    BOOST_FOREACH(auto ijk, boost::out_edges(ij, g))
    {
      VertexDesc ij = source(ijk, g);
      VertexDesc jk = target(ijk, g);

      if (!in_degree(ij, g) || !out_degree(jk, g))
      {
        continue;
      }

      OpSequence s = get_cheapest_elim(ijk, g);
      #pragma omp critical
      {
        if (s.cost() < opt.cost())
        {
          opt = s;
        }
      }
    }
  }

  return opt;
}

/**
 * @}
 */

}  // end namespace admission
