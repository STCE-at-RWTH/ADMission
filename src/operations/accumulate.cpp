// ################################ INCLUDES ################################ //

#include "operations/accumulate.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/properties.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/property_map/vector_property_map.hpp>

#include <map>
#include <tuple>
#include <utility>
#include <vector>

// ############################ SOURCE CONTENTS ############################# //

namespace admission
{

/******************************************************************************
 * @brief Uses an external property map to flag if a vertex is reachable
 *        from the source vertex of a DFS.
 ******************************************************************************/
struct reachable_detector : boost::default_dfs_visitor
{
  //! Refrence to the external property map.
  std::vector<bool>& reachable;

  /****************************************************************************
   * @brief Constructor.
   * @param[in] r Vector of bools as an external property map.
   ****************************************************************************/
  reachable_detector(std::vector<bool>& r) : reachable(r) {}

  /****************************************************************************
   * @brief The discover vertex function is the only
   *        one to override.
   * @param[in] v VertexDesctriptor of the disocvered vertex
   ****************************************************************************/
  template<typename V, typename G>
  void discover_vertex(V v, const G&)
  {
    reachable[v] = true;
  }
};

/******************************************************************************
 * @brief Accumulates the Jacobian of a DAG g by simply multiplying along
 *        all paths between all minimal \f$x \in X\f$ and all maximal
 *        \f$y \in Y\f$ vertices and adding the Jacobians \f$F'_{x,y}\f$
 *        of all paths.
 *
 * Constructs a tripartite face DAG \f$\tilde G'\f$ where each intermediate
 * vertex (x,y) holds the accumulated Jacobian \f$F'_{x,y}\f$.
 * g' is constructed by:
 *   For each minimal vertex \f$x \in X\f$
 *     Add a vertex \f$x' \in \tilde X' to \tilde g'\f$
 *   For each maximal vertex \f$y \in Y\f$
 *     Add a vertex \f$y' \in \tilde Y' to \tilde g'\f$
 *   For each edge \f$(x,y) \in E\f$
 *     Add a vertex \f$(x,y) \in \tilde V\f$
 *     Add an edge \f$(x', (x,y)) \in \tilde E\f$
 *     Add an edge \f$((x,y), y') \in \tilde E\f$
 * and the Jacobains on \f$(x,y) \in \tilde E\f$ are computed by:
 *   For each minimal vertex \f$x \in X\f$
 *     For each maximal vertex \f$y \in Y\f$
 *       For each path \f$(x,...,y)\f$
 *         Compute \f$F'_{x,...,y}\f$ the product of all elemantal Jacobians
 *         along \f$(x,...,y)\f$
 *       Compute \f$F'_{x,y}\f$ the sum of all \f$F'_{x,...,y}\f$
 *       and attach it to the corresponding vertex \f$(x,y) \in \tilde V\f$.
 *
 * @param[in] g reference to the original DAG.
 * @returns a shared_ptr to a tripartite face DAG derived from g.
 ******************************************************************************/
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
      boost::vector_property_map<boost::default_color_type> map;
      boost::depth_first_visit(g, x, rd, map);

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

std::shared_ptr<FaceDAG> accumulate_trivial(const FaceDAG& g)
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
      boost::vector_property_map<boost::default_color_type> map;
      boost::depth_first_visit(g, x, rd, map);

      BOOST_FOREACH(auto y, vertices(g))
      {
        if (out_degree(y, g) == 0 && v[y] == true && in_degree(y, g) > 0)
        {
          VertexDesc xy = add_vertex(tilde_g_dash);
          EdgeDesc xxy, xyy;
          std::tie(xxy, std::ignore) = add_edge(v2v[x], xy, tilde_g_dash);
          std::tie(xyy, std::ignore) = add_edge(xy, v2v[y], tilde_g_dash);

          boost::put(
              boost::edge_index, tilde_g_dash, xxy,
              boost::get(boost::edge_index, g, *out_edges(x, g).first));

          boost::put(
              boost::edge_size, tilde_g_dash, xxy,
              boost::get(boost::edge_size, g, *out_edges(x, g).first));

          boost::put(
              boost::edge_index, tilde_g_dash, xyy,
              boost::get(boost::edge_index, g, *in_edges(y, g).first));

          boost::put(
              boost::edge_size, tilde_g_dash, xyy,
              boost::get(boost::edge_size, g, *in_edges(y, g).first));

          boost::put(boost::vertex_acc_stat, tilde_g_dash, xy, true);
        }
      }
    }
  }
  return tilde_g_dash_ptr;
}

}  // end namespace admission

// ################################## EOF ################################### //
