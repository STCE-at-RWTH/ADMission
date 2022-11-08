// ******************************** Includes ******************************** //

#include "graph/generator.hpp"

#include "graph/DAG.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

// **************************** Source contents ***************************** //

namespace admission
{

template<typename Graph>
size_t GraphGenerator<Graph>::asc_dist(size_t min, size_t max)
{
  if (max < min)  // Assure argument correctnes.
  {
    throw std::runtime_error(
        "Invalid range [" + std::to_string(min) + "," + std::to_string(max) +
        "]");
  }
  return min + dist(1 + max - min);
}

template<typename Graph>
size_t GraphGenerator<Graph>::desc_dist(size_t min, size_t max)
{
  if (max < min)  // Assure argument correctnes.
  {
    throw std::runtime_error(
        "Invalid range [" + std::to_string(min) + "," + std::to_string(max) +
        "]");
  }
  return max - dist(1 + max - min);
}

/** Algorithm:
 *  Create all vertices. The vertex indices provide the odering of the graph
 *  i.e. edges (i,j) can only connect vertices i,j for which i < j.
 *
 *  Decide on a randum number of desired in- and out-edges of each vertex.
 *
 *  Connect all vertices according to the boundaries for the number of edges.
 *  The parameter _max_in_out determines the _desired_in and _desired_out
 *  property, but not neccesarily the the actual number of in- and out-edges
 *  of a vertex. It may happen that out_degree(i, g) > _desired_out[i].
 */
template<typename Graph>
void GraphGenerator<Graph>::generate_DAG(Graph& g)
{

  ADM_DEBUG(dag_generator_v) << "Allocating graph..." << std::endl;
  g = Graph(_p.num_vertices);  // Create a graph with num_vertices.
  _desired_in.resize(_p.num_vertices);  // Resize the helper containers.
  _desired_out.resize(_p.num_vertices);

  ADM_DEBUG(dag_generator_v)
      << "Generating random min max numbers..." << std::endl;
  size_t num_min = dist_t<size_t>(
      1, _p.num_vertices - _p.num_intermediate_vertices - 1)(_gen);
  size_t num_max = _p.num_vertices - _p.num_intermediate_vertices - num_min;
  ADM_DEBUG(dag_generator_v) << "num_min   = " << num_min << std::endl;
  ADM_DEBUG(dag_generator_v)
      << "_p.num_intermediate_vertices = " << _p.num_intermediate_vertices
      << std::endl;
  ADM_DEBUG(dag_generator_v) << "num_max   = " << num_max << std::endl;

  ADM_DEBUG(dag_generator_v) << "Generating random predecessor and successor "
                                "numbers for minimal verties"
                             << std::endl;

  // Number of minimum and intermediate vertices
  const size_t num_min_int = num_min + _p.num_intermediate_vertices;

  for (size_t i = 0; i < num_min; ++i)
  {
    // Minimal vertices have 0 predeccessors.
    _desired_in[i] = 0;

    // Minimal vertices have
    //  at least 1, maximum min{num_intermediate_vertices, max_in_out} successors,
    //  because we forbid edges between minimal and maximal vertices.
    _desired_out[i] = dist_t<size_t>(
        1, std::min(_p.num_intermediate_vertices, _p.max_in_out))(_gen);

    ADM_DEBUG(dag_generator_v)
        << "Desired out [" << i << "] = " << _desired_out[i] << std::endl;
  }

  ADM_DEBUG(dag_generator_v) << "Generating random predecessor and successor "
                                "numbers for intermediate vertices"
                             << std::endl;

  for (size_t i = num_min; i < num_min_int; ++i)
  {
    // An intermediate vertex i has at least 1 and max min{max_in_out, i}
    // predecessors, because vertex indices imply the ordering of the dag.
    // ie. (i,j) is an edge only if i < j.
    _desired_in[i] = dist_t<size_t>(1, std::min(i, _p.max_in_out))(_gen);

    // An intermediate vertex i has at least 1 and max min{max_in_out, num_vertices - 1 - i}
    // successotrs, because (i, j) is an edge only if i < j.
    _desired_out[i] = dist_t<size_t>(
        1, std::min(_p.num_vertices - i - 1, _p.max_in_out))(_gen);

    ADM_DEBUG(dag_generator_v)
        << "Desired in  [" << i << "] = " << _desired_in[i] << std::endl;
    ADM_DEBUG(dag_generator_v)
        << "Desired out [" << i << "] = " << _desired_out[i] << std::endl;
  }

  ADM_DEBUG(dag_generator_v) << "Generating random predecessor and successor "
                                "numbers for maximal vertices"
                             << std::endl;

  for (size_t i = num_min_int; i < num_min_int + num_max; ++i)
  {
    // A maximal vertex i has min 1 max min{max_in_out, num_intermediate_vertices}
    // predeccessors because we forbid edges connecting minimal to maximal vertices.
    _desired_in[i] = dist_t<size_t>(
        1, std::min(_p.max_in_out, _p.num_intermediate_vertices))(_gen);

    // Maximal vertices have no successors.
    _desired_out[i] = 0;

    ADM_DEBUG(dag_generator_v)
        << "Desired in [" << i << "] = " << _desired_in[i] << std::endl;
  }

  ADM_DEBUG(dag_generator_v)
      << "Adding successors to minimal vertices..." << std::endl;

  // For each minimal vertex i ...
  for (size_t i = 0; i < num_min; ++i)
  {
    // ...add  _desired_out[i] out-edges...
    for (size_t j = 0; j < _desired_out[i]; ++j)
    {
      // ...to a random intermediate vertex j.
      add_edge(i, desc_dist(num_min, num_min_int - 1), g);
    }
  }

  ADM_DEBUG(dag_generator_v)
      << "Adding predecessors to maximal vertices..." << std::endl;

  // For each maximal vertex j...
  for (size_t i = num_min_int; i < _p.num_vertices; ++i)
  {
    // ...add _desired_in[j] in-edges...
    for (size_t j = 0; j < _desired_in[i]; ++j)
    {
      // ...from a random intermediate vertex i.
      add_edge(asc_dist(num_min, num_min_int - 1), i, g);
    }
  }

  ADM_DEBUG(dag_generator_v)
      << "Adding succ/predecessors to intermediate vertices..." << std::endl;

  // For each intermediate vertex j...
  for (size_t j = num_min; j < num_min_int; ++j)
  {
    ADM_DEBUG(dag_generator_v)
        << "In  degree [" << j << "] = " << in_degree(j, g) << " Desired  in ["
        << j << "] = " << _desired_in[j] << std::endl;

    // While the _desired_in criterion is not satisfied...
    while (_desired_in[j] > in_degree(j, g))
    {
      int i = asc_dist(0, j - 1);
      add_edge(i, j, g);  // ...add in-edges.
      ADM_DEBUG(dag_generator_v)
          << "Adding  in edge (" << i << ", " << j << ")" << std::endl;
    }

    ADM_DEBUG(dag_generator_v)
        << "Out degree [" << j << "] = " << out_degree(j, g) << " Desired out ["
        << j << "] = " << _desired_out[j] << std::endl;

    // While the _desired_out criterion is not satisfied...
    while (_desired_out[j] > out_degree(j, g))
    {
      int k = desc_dist(j + 1, _p.num_vertices - 1);
      add_edge(j, k, g);  // ...add out-edges.
      ADM_DEBUG(dag_generator_v)
          << "Adding out edge (" << j << ", " << k << ")" << std::endl;
    }
  }
}

template<typename Graph>
void GraphGenerator<Graph>::annotate_DAG(Graph& g)
{
  dist_t<flop_t> random_s(1, _p.max_vertex_size);

  auto n = boost::get(boost::vertex_size, g);
  auto c_a = boost::get(boost::edge_c_adj, g);
  auto c_t = boost::get(boost::edge_c_tan, g);
  auto Fbardot_exists = boost::get(boost::edge_has_model, g);
  auto Fprime_exists = boost::get(boost::edge_acc_stat, g);

  BOOST_FOREACH(auto j, vertices(g))
  {
    n[j] = random_s(_gen);
  }

  BOOST_FOREACH(auto ij, edges(g))
  {
    auto& nj = n[boost::target(ij, g)];
    auto& ni = n[boost::source(ij, g)];

    /* Tangent and adjoint model costs are between
     * the sum of  *  *  *
     * the numbers  \ | /
     * of minimal     *
     * and maximal  / | \
     * vertices    *  *  *
     *
     * and
     *
     * the product      *  *  *
     * of the numbers   |\/|\/|
     *                  |/\|/\|
     * of minimal       *  *  *
     * and maximal      |\/|\/|
     * vertices         |/\|/\|
     * times a factor.  *  *  *
     */
    flop_t l = ni + nj;
    flop_t u = _p.cost_scaling * std::max(ni * nj, l);

    dist_t<flop_t> random_cost(l, u);
    c_a[ij] = random_cost(_gen);
    c_t[ij] = random_cost(_gen);
    Fbardot_exists[ij] = true;
    Fprime_exists[ij] = false;
  }
}

template<typename Graph>
void GraphGenerator<Graph>::operator()(Graph& g)
{
  ADM_DEBUG(dag_generator_v) << "Generating structure." << std::endl;
  generate_DAG(g);
  ADM_DEBUG(dag_generator_v) << "Annotating graph." << std::endl;
  annotate_DAG(g);
}

template class GraphGenerator<DAG>;

}  // end namespace admission
