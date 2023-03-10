#ifndef ADM_INC_GRAPH_IMPL_NEIGHBORS_HPP_
#define ADM_INC_GRAPH_IMPL_NEIGHBORS_HPP_

// ################################ INCLUDES ################################ //

#include "graph/neighbors.hpp"  // IWYU pragma: keep

#include <boost/foreach.hpp>

#include <algorithm>
#include <stddef.h>

// ###################### TEMPLATE AND INLINE CONTENTS ###################### //

namespace admission
{

/******************************************************************************
 * @brief Counts the number m of shared neighbors of two vertices
 *        \f$a,b \in V\f$.
 *
 * @tparam G Type of the graph.
 * @tparam Dir if Dir == lower_accessor \f$N(x) := P(x)\f$
 *             elseif Dir == upper_accessor \f$N(x) := S(x)\f$.
 * @param[in] g The face DAG.
 * @param[in] a Vertex.
 * @param[in] b Vertex.
 * @returns \f$|S(a) \cap S(b)|\f$
 ******************************************************************************/
template<typename G, typename Dir>
plength_t num_shared_neighbors(
    const G& g, const VertexDesc a, const VertexDesc b)
{
  size_t same_members = 0;
  BOOST_FOREACH(auto ea, Dir::edges(a, g))
  {
    BOOST_FOREACH(auto eb, Dir::edges(b, g))
    {
      if (Dir::next_vertex(ea, g) == Dir::next_vertex(eb, g))
      {
        same_members++;
      }
    }
  }
  return same_members;
}

/******************************************************************************
 * @brief Returns true, if the neighbor sets \f$N(a)\f$ and \f$N(b)\f$
 *        of vertices a and b are identical.
 *
 * @tparam G Type of the graph.
 * @tparam Dir If Dir == lower_accessor \f$N(x) := P(x)\f$ elseif
 *                Dir == upper_accessor \f$N(x) := S(x)\f$.
 * @param[in] g Const reference to the face DAG.
 * @param[in] a Vertex.
 * @param[in] b Vertex.
 * @returns \f$N(a) = N(b)\f$
 ******************************************************************************/
template<typename G, typename Dir>
ADM_ALWAYS_INLINE bool same_neighbors(
    const G& g, const VertexDesc a, const VertexDesc b)
{
  if (Dir::degree(a, g) != Dir::degree(b, g))
  {
    return false;
  }
  return (num_shared_neighbors<G, Dir>(g, a, b) == Dir::degree(a, g));
}

/******************************************************************************
 * @brief Returns true, if the neighbor sets \f$N(a)\f$ and \f$N(b)\f$
 *        of vertices a and b are identical except one vertex.
 *
 * @tparam Dir If Dir == lower_accessor \f$N(x) := P(x)\f$ elseif
 *                Dir == upper_accessor \f$N(x) := S(x)\f$.
 * @param[in] g Const reference to the face DAG.
 * @param[in] a Vertex.
 * @param[in] b Vertex.
 * @param[in] c Vertex.
 * @returns \f$|N(a)| - |N(a) \cap N(b)| = 1 \wedge c \notin N(b)\f$
 ******************************************************************************/
template<typename Dir>
bool same_neighbors_but_one(
    const FaceDAG& g, const VertexDesc a, const VertexDesc b,
    const VertexDesc c)
{
  // There must be exactly one neighbor of a that is not a neighbor of b.
  if (Dir::degree(a, g) - num_shared_neighbors<FaceDAG, Dir>(g, a, b) != 1)
  {
    return false;
  }

  // Check, that c (which is a neighbor of a)
  // is not a neighbor of b.
  BOOST_FOREACH(auto e, Dir::edges(b, g))
  {
    if (Dir::next_vertex(e, g) == c)
    {
      return false;
    }
  }
  return true;
}

/******************************************************************************
 * @brief Returns true, if the neighbor sets of vertices a and b overlap i.e.
 *        \f$N(a) \cup N(b) \neq N(a) \cap N(b) \neq 0\f$.
 *
 * @tparam Dir If Dir == lower_accessor \f$N(x) := P(x)\f$ elseif
 *                Dir == upper_accessor \f$N(x) := S(x)\f$.
 * @param[in] g Const reference to the face DAG.
 * @param[in] a Vertex desc of the first vertex to compare
 * @param[in] b Vertex desc of the second vertex to compare
 * @returns boolean true if \f$N(a) \cup N(b) \neq N(a) \cap N(b) \neq 0\f$,
 * else otherwise.
 ******************************************************************************/
template<typename Dir>
ADM_ALWAYS_INLINE bool share_few_neighbors(
    const FaceDAG& g, const VertexDesc a, const VertexDesc b)
{
  auto n = num_shared_neighbors<FaceDAG, Dir>(g, a, b);
  return (0 < n && n < std::max(Dir::degree(a, g), Dir::degree(b, g)));
}

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_GRAPH_IMPL_NEIGHBORS_HPP_
