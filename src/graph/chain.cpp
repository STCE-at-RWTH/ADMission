// ################################ INCLUDES ################################ //

#include "graph/chain.hpp"

#include "admission_config.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <algorithm>
#include <ostream>
#include <stddef.h>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

// The drag interface
#include <drag/drag.hpp>

// ############################ SOURCE CONTENTS ############################# //

namespace admission
{

/******************************************************************************
 * @brief Constructs a Jacobian chain from a DAG.
 *
 * @param[in] g DAG to construct the chain from.
 * @returns std::shared_ptr to the chain.
 ******************************************************************************/
std::shared_ptr<admission::DAG> make_chain(admission::DAG& g)
{
  using namespace drag;
  namespace detail = drag::detail;
  std::shared_ptr<DAG> cp = std::make_shared<DAG>();  // The result DAG.
  DAG& c = *cp;  // Reference to result for convenience.
  EdgeDesc e, ij;  // e is a chain DAG edge, ij is a DAG edge.
  VertexDesc l, i;  // l is a chain layer, i is a DAG vertex.
  graph d;  // A drag graph required as input of the layering.

  /* Build a drag graph d corresponding to g. */
  BOOST_FOREACH(i, vertices(g))
  {
    d.add_node();
  }

  BOOST_FOREACH(ij, edges(g))
  {
    d.add_edge(source(ij, g), target(ij, g));
  }

  /* Split d. This is just for the sake of the interface.
   * We do not expect that g consists of two disjoint subgraphs. */
  auto ds = detail::split(d);
  if (ds.size() != 1)
  {
    throw std::runtime_error(
        "The DAG to layer consists of multiple disconnected subgraphs!");
  }

  /* Run the layering algorithm. */
  auto nsl = std::make_unique<detail::network_simplex_layering>();
  detail::hierarchy h = nsl->run(ds[0]);
  detail::add_dummy_nodes(h);
  ADM_DEBUG(dag_io_v) << "Number of generated layers: " << h.layers.size()
                      << std::endl;

  /* Do some check that all minimal and maximal vertices
   * are on the lowest and highest layer respectively. */
  BOOST_FOREACH(i, vertices(g))
  {
    if ((in_degree(i, g) == 0) && (h.ranking[i] != 0))
    {
      throw std::runtime_error("A minimal vertex is not on layer 0!");
    }
    else if (
        (out_degree(i, g) == 0) &&
        (size_t(h.ranking[i]) != h.layers.size() - 1))
    {
      throw std::runtime_error(
          "A maximal vertex is not on layer " +
          std::to_string(h.layers.size() - 1) + "!");
    }
  }

  /* Property maps for easy copying of properties. */
  auto c_n = boost::get(boost::vertex_size, c);
  auto c_c_tan = boost::get(boost::edge_c_tan, c);
  auto c_c_adj = boost::get(boost::edge_c_adj, c);
  auto g_n = boost::get(boost::vertex_size, g);
  auto g_c_tan = boost::get(boost::edge_c_tan, g);
  auto g_c_adj = boost::get(boost::edge_c_adj, g);

  /* Copy the layering to a new DAG c and initialize its properties to 0 */
  ADM_DEBUG(dag_io_v) << "Generating layered DAG c" << std::endl;
  for (l = 0; l < h.layers.size(); ++l)
  {
    add_vertex(c);
    c_n[l] = 0;
    ADM_DEBUG(dag_io_v) << "  Adding vertex " << l << std::endl;
    ADM_DEBUG(dag_io_v) << "  n[" << l << "] = " << c_n[l] << std::endl;
    if (l >= 1)
    {
      std::tie(e, std::ignore) = add_edge(l - 1, l, c);
      c_c_tan[e] = 0;
      c_c_adj[e] = 0;
      ADM_DEBUG(dag_io_v) << "  Adding edge (" << l - 1 << ", " << l << ")"
                          << std::endl;
      ADM_DEBUG(dag_io_v) << "  c_tan[(" << l - 1 << ", " << l
                          << ")] = " << c_c_tan[e] << std::endl;
      ADM_DEBUG(dag_io_v) << "  c_adj[(" << l - 1 << ", " << l
                          << ")] = " << c_c_adj[e] << std::endl;
    }
  }

  /* Add the size n of each vertex to its corresponding layer. */
  BOOST_FOREACH(i, vertices(g))
  {
    c_n[h.ranking[i]] += g_n[i];
    ADM_DEBUG(dag_io_v) << "Updating layer " << h.ranking[i] << " from vertex "
                        << i << std::endl;
    ADM_DEBUG(dag_io_v) << "  n[" << h.ranking[i] << "] = " << c_n[h.ranking[i]]
                        << std::endl;
  }

  /* Add the edges. */
  BOOST_FOREACH(ij, edges(g))
  {
    VertexDesc i = source(ij, g);
    VertexDesc j = target(ij, g);

    if (h.span(i, j) == 1)
    {
      /* If an edge has a span of 1 (connects two consecutive layers), just add
       * the cost. */

      ADM_DEBUG(dag_io_v) << "Updating edge with span 1" << std::endl;
      std::tie(e, std::ignore) = boost::edge(h.ranking[i], h.ranking[j], c);
      c_c_adj[e] += g_c_adj[ij];
      c_c_tan[e] += g_c_tan[ij];

      ADM_DEBUG(dag_io_v) << "Updating layers (" << h.ranking[i] << ", "
                          << h.ranking[j] << ") from edge (" << i << ", " << j
                          << ")" << std::endl;
      ADM_DEBUG(dag_io_v) << "  c_tan[(" << h.ranking[i] << ", " << h.ranking[j]
                          << ")] = " << c_c_tan[e] << std::endl;
      ADM_DEBUG(dag_io_v) << "  c_adj[(" << h.ranking[i] << ", " << h.ranking[j]
                          << ")] = " << c_c_adj[e] << std::endl;
    }
    else
    {
      /* If an edge spans multiple layers, where to place the calculation.
       * All intermediate layers are filled with the smaller vertex and all
       * edges connecting those do not receive any additional cost.
       *
       * Consider the edge (i,j) with a span of 4 and n(i) > n(j)
       * layer | size from (i,j)
       *   4   |     n(j)       j
       *   3   |     n(j)        ^
       *   2   |     n(j)         \
       *   1   |     n(j)          \
       *   0   |     n(i)           i
       *
       * layer edge | cost from (i,j)
       *   (3,4)    |       0
       *   (2,3)    |       0
       *   (1,2)    |       0
       *   (0,1)    |     c(i,j)
       */

      const auto n = std::min(g_n[i], g_n[j]);

      std::tie(
          e, std::ignore) = (n == g_n[i]) ?
                                boost::edge(h.ranking[j] - 1, h.ranking[j], c) :
                                boost::edge(h.ranking[i], h.ranking[i] + 1, c);
      c_c_tan[e] += g_c_tan[ij];
      c_c_adj[e] += g_c_adj[ij];
      ADM_DEBUG(dag_io_v) << "Updating edge with span " << h.span(i, j)
                          << std::endl;
      ADM_DEBUG(dag_io_v) << "Updating layers (...) from edge (" << i << ", "
                          << j << ")" << std::endl;
      ADM_DEBUG(dag_io_v) << "  c_tan[(...)] = " << c_c_tan[e] << std::endl;
      ADM_DEBUG(dag_io_v) << "  c_adj[(...)] = " << c_c_adj[e] << std::endl;

      const VertexDesc start = h.ranking[i] + (n != g_n[i]) + 1;
      const VertexDesc end = h.ranking[j] + (n == g_n[i]) - 1;
      for (l = start; l < end; ++l)
      {
        c_n[l] += n;
        ADM_DEBUG(dag_io_v) << "Updating layer " << l << " from vertex pair"
                            << i << ", " << j << std::endl;
        ADM_DEBUG(dag_io_v) << "  n[" << l << "] = " << c_n[l] << std::endl;
      }
    }
  }
  return cp;
}

}  // end namespace admission

// ################################## EOF ################################### //
