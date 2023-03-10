// ################################ INCLUDES ################################ //

#include "graph/face_dag.hpp"

#include "admission_config.hpp"
#include "graph/min_max.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>

#include <assert.h>
#include <list>
#include <map>
#include <utility>

// ############################ SOURCE CONTENTS ############################# //

namespace admission
{

/******************************************************************************
 * @brief Adds a vertex to a graph and initialises it with the
 *        corresponding edge from the source DAG.
 *
 * Follows rule from (Definition 3) (1. 3. 5.).
 *
 * @param[in] g const reference to the graph we come from.
 * @param[in] e edge used to initiaise the vertex.
 * @param[inout] g_face reference to the graph we add an edge to.
 * @returns a descriptor of the new vertex.
 ******************************************************************************/
ADM_ALWAYS_INLINE VertexDesc
add_vertex_from(DAG& g, const EdgeDesc e, FaceDAG& g_face)
{
  auto w = add_vertex(g_face);
  boost::put(
      boost::vertex_acc_stat, g_face, w,
      boost::get(boost::edge_acc_stat, g, e));
  boost::put(
      boost::vertex_has_model, g_face, w,
      boost::get(boost::edge_has_model, g, e));
  boost::put(
      boost::vertex_c_tan, g_face, w, boost::get(boost::edge_c_tan, g, e));
  boost::put(
      boost::vertex_c_adj, g_face, w, boost::get(boost::edge_c_adj, g, e));
  return w;
}

/******************************************************************************
 * @brief Adds an edge to a FaceDAG and initialises it with the
 *        corresponding vertex from the DAG.
 *
 * Follows rule from (Definition 3) (2. 4. 6.).
 *
 * @param[in] g const reference to the graph we come from.
 * @param[in] v vertex used to initiaise the new edge.
 * @param[inout] g_face reference to the graph we add an edge to.
 * @param[in] i source of the new edge.
 * @param[in] j target of the new edge.
 * @returns a descriptor of the new edge.
 ******************************************************************************/
ADM_ALWAYS_INLINE std::pair<EdgeDesc, bool> add_edge_from(
    const DAG& g, const VertexDesc v, FaceDAG& g_face, const VertexDesc i,
    const VertexDesc j)
{
  auto e = add_edge(i, j, g_face);
  boost::put(
      boost::edge_index, g_face, e.first,
      boost::get(boost::vertex_index, g, v));
  boost::put(
      boost::edge_size, g_face, e.first, boost::get(boost::vertex_size, g, v));
  return e;
}

/******************************************************************************
 * @brief Constructs a face DAG from a DAG according to the Rule in
 *        (Definition 3).
 *
 * @param[in] g_const Const reference to a DAG.
 * @returns std::shared_ptr to the face DAG corresponding to g_const.
 ******************************************************************************/
std::shared_ptr<FaceDAG> make_face_dag(const DAG& g_const)
{
  // Copy g_const, because we modify the source DAG to simplify the
  // construction.
  DAG g = g_const;

  // Find vertices without succ. or pred. aka. maximal and minimal vertices.
  auto min_v = min_vertices(g);  // X' = {i \in V : P(i) = \emptyset}
  auto max_v = max_vertices(g);  // Y' = {i \in V : S(i) = \emptyset}

  // Augment g with additional minimal and maximal vertices.
  // This enables us to just convert all edges (ij) \in E
  // to vertices (ij) \in \tilde E.
  // \forall i \in X' do V := V u {i^-} and E := E u {(i^-, i)}
  for (auto j : min_v)
  {
    auto i = add_vertex(g);
    add_edge(i, j, g);
  }

  // \forall i \in Y' do V := V u {i^+} and E := E u {(i, i^+)}
  for (auto j : max_v)
  {
    auto k = add_vertex(g);
    add_edge(j, k, g);
  }

  // Create the new face DAG.
  std::shared_ptr<FaceDAG> gfp = std::make_shared<FaceDAG>();

  std::map<EdgeDesc, VertexDesc> e2v;
  BOOST_FOREACH(auto orig_edge, edges(g))
  {
    e2v.emplace(orig_edge, admission::add_vertex_from(g, orig_edge, *gfp));
  }

  BOOST_FOREACH(auto orig_edge, edges(g))
  {
    auto curr = e2v.find(orig_edge);
    BOOST_FOREACH(auto orig_succ_edge, out_edges(target(orig_edge, g), g))
    {
      auto succ = e2v.find(orig_succ_edge);
      assert(succ != e2v.end() && "Did not find this edge!");
      admission::add_edge_from(
          g, target(orig_edge, g), *gfp, curr->second, succ->second);
    }
  }
  return gfp;
}

}  // end namespace admission

// ################################## EOF ################################### //
