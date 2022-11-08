#ifndef GLOBAL_MODES_HPP
#define GLOBAL_MODES_HPP

// ******************************** Includes ******************************** //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "operations/find_eliminations.hpp"
#include "operations/op_sequence.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <stdexcept>
#include <utility>

// **************************** Header contents ***************************** //

namespace admission
{

/**\ingroup DagInfo
 * \brief Computes the cost of global tangent mode on a dense FaceDAG.
 *
 * @param g Reference to the face DAG.
 * @return flop_t Cost of the global tangent mode on g.
 */
ADM_ALWAYS_INLINE
flop_t global_tangent_cost(const FaceDAG& g)
{
  flop_t size = 0;
  flop_t cost_sum = 0;

  auto c = boost::get(boost::vertex_c_tan, g);
  auto n = boost::get(boost::edge_size, g);

  auto Fprime_exists = boost::get(boost::vertex_acc_stat, g);
  auto Fbardot_exists = boost::get(boost::vertex_has_model, g);

  BOOST_FOREACH(auto v, vertices(g))
  {
    if (in_degree(v, g) && out_degree(v, g))
    {
      if (Fbardot_exists[v])
      {
        cost_sum += c[v];
      }
      else if (Fprime_exists[v])
      {
        cost_sum += n[*(in_edges(v, g).first)] * n[*(out_edges(v, g).first)];
      }
      else
      {
        throw std::runtime_error(
            "An edge has neither a Jacobian nor tan/adj models!");
      }
    }
    else if ((!in_degree(v, g)) && out_degree(v, g))
    {
      size += n[*(out_edges(v, g).first)];
    }
  }
  return size * cost_sum;
}

/**\ingroup DagInfo
 * \brief Computes the cost of global adjoint mode on a dense FaceDAG.
 *
 * @param g Reference to the face DAG.
 * @return flop_t Cost of the global adjoint mode on g.
 */
ADM_ALWAYS_INLINE
flop_t global_adjoint_cost(const FaceDAG& g)
{
  flop_t size = 0;
  flop_t cost_sum = 0;

  auto c = boost::get(boost::vertex_c_adj, g);
  auto n = boost::get(boost::edge_size, g);

  auto Fprime_exists = boost::get(boost::vertex_acc_stat, g);
  auto Fbardot_exists = boost::get(boost::vertex_has_model, g);

  BOOST_FOREACH(auto v, vertices(g))
  {
    if (in_degree(v, g) && out_degree(v, g))
    {
      if (Fbardot_exists[v])
      {
        cost_sum += c[v];
      }
      else if (Fprime_exists[v])
      {
        cost_sum += n[*(in_edges(v, g).first)] * n[*(out_edges(v, g).first)];
      }
      else
      {
        throw std::runtime_error(
            "An edge has neither a Jacobian nor tan/adj models!");
      }
    }
    else if (in_degree(v, g) && (!out_degree(v, g)))
    {
      size += n[*(in_edges(v, g).first)];
    }
  }
  return size * cost_sum;
}

/**\ingroup DagInfo
 * \brief Computes the cost of global optimal preaccumulation of all intermediate Jacobians.
 * @param g Reference to the face DAG.
 * @return flop_t Cost of preaccumulation of every elemental Jacobian.
 */
ADM_ALWAYS_INLINE
OpSequence global_preaccumulation_ops(const FaceDAG& g)
{
  OpSequence res = OpSequence::make_empty();

  auto Fprime_exists = boost::get(boost::vertex_acc_stat, g);
  auto Fbardot_exists = boost::get(boost::vertex_has_model, g);

  BOOST_FOREACH(auto v, vertices(g))
  {
    if (in_degree(v, g) && out_degree(v, g) && (!Fprime_exists[v]) &&
        Fbardot_exists[v])
      res += cheapest_preacc(v, g);
  }
  return res;
}

}  // end namespace admission

#endif  // GLOBAL_MODES_HPP
