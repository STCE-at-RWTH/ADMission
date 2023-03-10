// ################################ INCLUDES ################################ //

#include "graph/verify.hpp"

#include "admission_config.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <tuple>
#include <type_traits>

// ############################# SOURCE CONTENTS ############################ //

namespace admission
{

/******************************************************************************
 * @brief Verifies that two face DAGs \f$a = (\tilde V_a, \tilde E_a)\f$ and
 *        \f$b = (\tilde V_b, \tilde E_b)\f$ are equal.
 *
 * Check for identical elemental Jacobians:
 *   For each intermediate vertex \f$(x,y) \in \tilde V_a\f$.
 *     Find the corresponding vertex \f$(x,y) \in \tilde V_b\f$.
 *     Assert that the in- and out-degrees of both vertices are 1.
 *     Assert that \f$n_a((x, (x,y))) == n_b((x, (x,y)))\f$ and
 *       \f$n_a(((x,y), y)) == n_b(((x,y), y))\f$.
 *
 * @param[in] a First Graph
 * @param[in] b Second Graph
 * @returns A pair of bool (true, if the graphs are equal) and string (a
 *          description of the first difference between a and b that was found).
 ******************************************************************************/
std::pair<bool, std::string> verify_equal_DAGs(
    admission::FaceDAG& a, admission::FaceDAG& b)
{
  auto ind = boost::get(boost::edge_index, a);

  /* Removes all isolated vertices from a face DAG, because eliminate_edge
   * will not delete isolated vertices for the sake of speed. */
  auto remove_isolated = [&](admission::FaceDAG& g)
  {
    auto remove_one = [&]()
    {
      BOOST_REVERSE_FOREACH(auto v, vertices(g))
      {
        if ((in_degree(v, g) == 0) && (out_degree(v, g) == 0))
        {
          remove_vertex(v, g);
          return false;
        }
      }
      return true;
    };
    while (remove_one() == false)
    {}
  };

  remove_isolated(a);
  remove_isolated(b);

  BOOST_FOREACH(auto v, vertices(a))
  {
    if ((out_degree(v, a) == 1) && (in_degree(v, a) == 1))
    {
      auto vi = index_pair(v, a);
      bool found_match = false;

      BOOST_FOREACH(auto w, vertices(b))
      {
        if ((out_degree(w, b) == 1) && (in_degree(w, b) == 1) &&
            (index_pair(w, b) == vi))
        {
          found_match = true;
        }
      }
      if (!found_match)
      {
        return std::make_pair(
            false, "No match found for vertex (" +
                       std::to_string(ind[*(in_edges(v, a).first)]) + ", " +
                       std::to_string(ind[*(out_edges(v, a).first)]) +
                       ") of graph a!");
      }
    }
  }
  return std::make_pair(true, "OK");
}

}  // end namespace admission

// ################################## EOF ################################### //
