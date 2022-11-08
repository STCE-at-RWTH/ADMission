#ifndef NEIGHBORS_HPP
#define NEIGHBORS_HPP

// ******************************** Includes ******************************** //

#include "admission_config.hpp"
#include "graph/DAG.hpp"

#include <boost/foreach.hpp>

#include <algorithm>
#include <stddef.h>

// **************************** Header contents ***************************** //

namespace admission
{

/**
 * \addtogroup DagInfo
 * @{
 */

/**\brief Counts the number m of shared neighbors of two vertices \f$a,b \in V\f$.
 *
 * @tparam dir If Dir == lower_accessor \f$N(x) := P(x)\f$ elseif Dir == upper_accessor \f$N(x) := S(x)\f$.
 * @param[in] g the face DAG.
 * @param[in] a vertex.
 * @param[in] b vertex.
 * @returns \f$|S(a) \cap S(b)|\f$
 */
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

/**\brief Returns true, if the neighbor sets \f$N(a)\f$ and \f$N(b)\f$
 * of vertices a and b are identical.
 *
 * @tparam dir If Dir == lower_accessor \f$N(x) := P(x)\f$ elseif Dir == upper_accessor \f$N(x) := S(x)\f$.
 * @param g const reference to the face DAG.
 * @param a vertex.
 * @param b vertex.
 * @return \f$N(a) == N(b)\f$
 */
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

/**\brief Returns true, if the neighbor sets of vertices a and b overlap i.e.
 *
 * \f$N(a) \cup N(b) \neq N(a) \cap N(b) \neq 0\f$
 * @tparam dir If Dir == lower_accessor \f$N(x) := P(x)\f$ elseif Dir == upper_accessor \f$N(x) := S(x)\f$.
 * @param g const reference to the face DAG.
 * @param a vertex desc of the first vertex to compare
 * @param b vertex desc of the second vertex to compare
 * @return boolean
 */
template<typename Dir>
ADM_ALWAYS_INLINE bool share_few_neighbors(
    const FaceDAG& g, const VertexDesc a, const VertexDesc b)
{
  auto n = num_shared_neighbors<FaceDAG, Dir>(g, a, b);
  return (0 < n && n < std::max(Dir::degree(a, g), Dir::degree(b, g)));
}

/**\brief Explicit instantiation of \ref same_neighbors() to compare
 *        predecessor sets \f$|P(i)|\f$ and \f$|P(j)|\f$.
 */
const auto same_predecessors = same_neighbors<FaceDAG, lower_accessor>;

/**\brief Explicit instantiation of \ref same_neighbors() to compare
 *        successor sets \f$|P(i)|\f$ and \f$|P(j)|\f$.
 */
const auto same_successors = same_neighbors<FaceDAG, upper_accessor>;

/**\brief Explicit instantiation of \ref same_neighbors_but_one() to compare
 *        predecessor sets \f$|P(i)|\f$ and \f$|P(j)|\f$.
 */
const auto same_predecessors_but_one = same_neighbors_but_one<lower_accessor>;

/**\brief Explicit instantiation of \ref same_neighbors_but_one() to compare
 *        successor sets \f$|P(i)|\f$ and \f$|P(j)|\f$.
 */
const auto same_successors_but_one = same_neighbors_but_one<upper_accessor>;

/**\brief Explicit instantiation of \ref share_few_neighbors() to compare
 *        successor sets \f$|P(i)|\f$ and \f$|P(j)|\f$.
 */
const auto share_few_predecessors = share_few_neighbors<lower_accessor>;

/**\brief Explicit instantiation of \ref share_few_neighbors() to compare
 *        successor sets \f$|P(i)|\f$ and \f$|P(j)|\f$.
 */
const auto share_few_successors = share_few_neighbors<upper_accessor>;

/**
 * @}
 */

}  // end namespace admission

#endif  // NEIGHBORS_HPP
