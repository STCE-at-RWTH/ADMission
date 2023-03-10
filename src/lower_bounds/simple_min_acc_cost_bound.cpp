// ################################ INCLUDES ################################ //

#include "lower_bounds/simple_min_acc_cost_bound.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/property_map/property_map.hpp>

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

// ############################ SOURCE CONTENTS ############################# //

namespace admission
{

/******************************************************************************
 * @brief Applies the lower bound to a face DAG.
 *
 * @param g Reference to the DAG we compute the bound on.
 * @returns A valid lower bound for matrix-free FE on g.
 ******************************************************************************/
flop_t SimpleMinAccCostBound::operator()(const FaceDAG& g) const
{

  flop_t min_acc_c = 0;
  std::vector<flop_t> uf(num_vertices(g), std::numeric_limits<flop_t>::max());
  std::vector<flop_t> lf(num_vertices(g), std::numeric_limits<flop_t>::max());
  std::vector<bool> was_used(num_vertices(g), false);

  auto c_adj = boost::get(boost::vertex_c_adj, g);
  auto c_tan = boost::get(boost::vertex_c_tan, g);
  auto n = boost::get(boost::edge_size, g);
  auto Fdot_tilde_exists = boost::get(boost::vertex_has_model, g);
  auto Fprime_exists = boost::get(boost::vertex_acc_stat, g);

  auto ufm = boost::make_iterator_property_map(
      uf.begin(), boost::get(boost::vertex_index, g));
  auto lfm = boost::make_iterator_property_map(
      lf.begin(), boost::get(boost::vertex_index, g));

  BOOST_FOREACH(auto e, edges(g))
  {
    if (out_degree(target(e, g), g) == 0)
    {
      put_extreme(g, e, lf, lower_accessor());
    }
    else if (in_degree(source(e, g), g) == 0)
    {
      put_extreme(g, e, uf, upper_accessor());
    }
  }

  BOOST_FOREACH(auto v, vertices(g))
  {

    if (in_degree(v, g) && out_degree(v, g) && Fprime_exists[v] == false &&
        Fdot_tilde_exists[v] == true)
    {
      min_acc_c += std::min(
          {c_adj[v] * n[*(out_edges(v, g).first)],
           c_tan[v] * n[*(in_edges(v, g).first)], c_adj[v] * ufm[v],
           c_tan[v] * lfm[v]});
    }
  }

  ADM_DEBUG(5) << "LB = " << min_acc_c << std::endl;
  return min_acc_c;
}

}  // end namespace admission

// ################################## EOF ################################### //
