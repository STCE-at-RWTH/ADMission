#ifndef ADM_INC_OPERATIONS_IMPL_ELIMINATION_ALGORITHMS_HPP_
#define ADM_INC_OPERATIONS_IMPL_ELIMINATION_ALGORITHMS_HPP_

// ################################ INCLUDES ################################ //

#include "graph/DAG.hpp"
#include "graph/neighbors.hpp"
#include "operations/elimination_algorithms.hpp"  // IWYU pragma: keep

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <tuple>

// ###################### TEMPLATE AND INLINE CONTENTS ###################### //

namespace admission
{

/******************************************************************************
 * @brief Checks if a vertex that is part of an elimination
 *        will (can) be merged.
 *
 * @tparam Acc Type of the accessor functor to determine the direction in which
 *         we work.
 * @param[in] to_merge Source or target of the edge to be eliminated.
 * @param[in] to_ignore The other vertex of the edge to be eliminated.
 * @param[in] g Const reference to the DAG.
 * @returns tuple of bool bool VertexDesc First bool is true if a merge
 *          candidate exists, Second bool is true if the candidate is
 *          accumulated, VertexDesc is that vertex and otherwise
 *          default-initialised.
 ******************************************************************************/
template<typename Acc>
std::tuple<bool, bool, VertexDesc> check_merge(
    const VertexDesc to_merge, const VertexDesc to_ignore, const FaceDAG& g)
{
  bool exists = false;
  bool is_preacc = false;
  VertexDesc res = VertexDesc();

  // Loop over all successors (ik) of (to_merge)
  BOOST_FOREACH(auto oe, Acc::edges(to_merge, g))
  {
    VertexDesc jprimekprime = Acc::next_vertex(oe, g);

    // ... and over all predecessors (h,j) of all successors (j,k).
    BOOST_FOREACH(auto ie, Acc::reverse::edges(jprimekprime, g))
    {
      VertexDesc candidate = Acc::reverse::next_vertex(ie, g);

      if (candidate == to_merge)
      {
        continue;
      }

      // Check if succ. set and pred. set of j and v are identical
      // P(i,*,j) = P(*,j) && S(i,*,j) = S(*,j)
      if (same_neighbors_but_one<Acc>(g, candidate, to_merge, to_ignore) &&
          same_neighbors<FaceDAG, typename Acc::reverse>(
              g, candidate, to_merge))
      {
        // F'(*,j) != \emptyset
        if (get(boost::vertex_acc_stat, g, candidate))
        {
          exists = true;
          is_preacc = true;
          res = candidate;
        }
        else if (!is_preacc && !exists)
        {
          exists = true;
          res = candidate;
        }
      }
    }
  }
  return std::make_tuple(exists, is_preacc, res);
}

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_OPERATIONS_IMPL_ELIMINATION_ALGORITHMS_HPP_
