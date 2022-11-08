// ******************************** Includes ******************************** //

#include "operations/accumulate.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/properties.hpp>

#include <assert.h>
#include <list>
#include <map>
#include <tuple>
#include <vector>

// **************************** Source contents ***************************** //

namespace admission
{

/**
 * Recursively finds all paths between two vertices.
 * @param[in] g const reference to the graph we operate on
 * @param[in] from VertexDesc of the source vertex
 * @param[in] to   VertexDesc of the target vertex
 * @returns a list of paths between from and to. Each path is a list of the
 *          EdgeDesc of the edges of the path.
 */
std::list<std::list<EdgeDesc>> path_from_to(
    const DAG& g, VertexDesc from, VertexDesc to)
{
  std::list<std::list<EdgeDesc>> sp;

  BOOST_FOREACH(auto ed, out_edges(from, g))
  {
    auto next = target(ed, g);
    if (next == to)
    {
      sp.emplace_back();
      sp.back().push_back(ed);
      return sp;
    }
    else
    {
      std::list<std::list<EdgeDesc>> lps = path_from_to(g, next, to);
      if (!lps.empty())
      {
        for (auto& p : lps)
        {
          assert(!p.empty() && "Path is empty!");
          p.push_front(ed);
        }
      }

      for (auto& p : lps)
      {
        sp.push_back(p);
      }
    }
  }
  return sp;
}

/**\brief Uses an external property map to flag if a vertex is reachable
 *        from the source vertex of a DFS.
 */
struct reachable_detector : boost::default_dfs_visitor
{
  /// Refrence to the external property map.
  std::vector<bool>& reachable;

  /// Constructor.
  reachable_detector(std::vector<bool>& r) : reachable(r) {}

  /**\brief The discover vertex function is the only
   *        one to override.
   */
  template<typename V, typename G>
  void discover_vertex(V v, G const&)
  {
    reachable[v] = true;
  }
};

/**\brief Accumulates the Jacobian of a DAG g by simply multiplying along
 * all paths between all minimal \f$x \in X\f$ and all maximal \f$y \in Y\f$
 * vertices and adding the Jacobians \f$F'_{x,y}\f$ of all paths.
 * Constructs a tripartite face DAG \f$\tilde G'\f$ where each intermediate vertex (x,y)
 * holds the accumulated Jacobian \f$F'_{x,y}\f$.
 * g' is constructed by:
 *     For each minimal vertex x \in X
 *      Add a vertex \f$x' \in \tilde X' to \tilde g'\f$
 *     For each maximal vertex \f$y \in Y\f$
 *      Add a vertex \f$y' \in \tilde Y' to \tilde g'\f$
 *     For each edge \f$(x,y) \in E\f$
 *      Add a vertex \f$(x,y) \in \tilde V\f$
 *      Add an edge \f$(x', (x,y)) \in \tilde E\f$
 *      Add an edge \f$((x,y), y') \in \tilde E\f$
 * and the Jacobains on \f$(x,y) \in \tilde E\f$ are computed by:
 *   For each minimal vertex \f$x \in X\f$
 *     For each maximal vertex \f$y \in Y\f$
 *       For each path \f$(x,...,y)\f$
 *         Compute \f$F'_{x,...,y}\f$ the product of all elemantal Jacobians along \f$(x,...,y)\f$
 *       Compute \f$F'_{x,y} the sum of all \f$F'_{x,...,y}\f$
 *       and attach it to the corresponding vertex \f$(x,y) \in \tilde V\f$.
 *
 * @param[in] g reference to the original DAG.
 * @returns a shared_ptr to a tripartite face DAG derived from g.
 */
std::shared_ptr<FaceDAG> accumulate_trivial(const DAG& g)
{
  auto tilde_g_dash_ptr = std::make_shared<FaceDAG>();
  FaceDAG& tilde_g_dash = *tilde_g_dash_ptr;

  std::map<VertexDesc, VertexDesc> v2v;
  BOOST_FOREACH(auto v, vertices(g))
  {
    if (out_degree(v, g) == 0 || in_degree(v, g) == 0)
    {
      v2v.emplace(v, add_vertex(tilde_g_dash));
    }
  }

  BOOST_FOREACH(auto x, vertices(g))
  {
    if (in_degree(x, g) == 0)
    {
      std::vector<bool> v(num_vertices(g), false);
      reachable_detector rd(v);
      boost::depth_first_search(g, boost::visitor(rd).root_vertex(x));

      BOOST_FOREACH(auto y, vertices(g))
      {
        if (out_degree(y, g) == 0 && v[y] == true)
        {
          VertexDesc xy = add_vertex(tilde_g_dash);
          EdgeDesc xxy, xyy;
          std::tie(xxy, std::ignore) = add_edge(v2v[x], xy, tilde_g_dash);
          std::tie(xyy, std::ignore) = add_edge(xy, v2v[y], tilde_g_dash);

          boost::put(
              boost::edge_index, tilde_g_dash, xxy,
              boost::get(boost::vertex_index, g, x));
          boost::put(
              boost::edge_size, tilde_g_dash, xxy,
              boost::get(boost::vertex_size, g, x));

          boost::put(
              boost::edge_index, tilde_g_dash, xyy,
              boost::get(boost::vertex_index, g, y));
          boost::put(
              boost::edge_size, tilde_g_dash, xyy,
              boost::get(boost::vertex_size, g, y));

          boost::put(boost::vertex_acc_stat, tilde_g_dash, xy, true);
        }
      }
    }
  }

  return tilde_g_dash_ptr;
}

}  // end namespace admission
