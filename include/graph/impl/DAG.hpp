#ifndef ADM_INC_GRAPH_IMPL_DAG_HPP_
#define ADM_INC_GRAPH_IMPL_DAG_HPP_

// ################################ INCLUDES ################################ //

#include "graph/DAG.hpp"  // IWYU pragma: keep

// ###################### TEMPLATE AND INLINE CONTENTS ###################### //

namespace admission
{

/******************************************************************************
 * @brief Wrapper for returning the index of an edge,
 *        relieving us from the boost::get syntax.
 *
 * Phew...
 ******************************************************************************/
template<typename G, typename E>
ADM_ALWAYS_INLINE const index_t& index(const E e, const G& g)
{
  return boost::get(boost::edge_index, g, e);
}

/******************************************************************************
 * @brief Converts an EdgeDesc e of a FaceDAG g to the index triple
 *        \f$(i,j,k) \in \tilde E\f$ defining an edge of a FaceDAG \f$\tilde
 *        G\f$.
 *
 * @attention index_triple(...) must be called on an intermediate edge,
 *            because the iterators returned by in_edges(...) and out_edges(...)
 *            are dereferenced without checking their validity.
 *
 * @param[in] e EdgeDesc of the edge whose index triple we want.
 * @param[in] g Const reference to the FaceDAG.
 * @returns 3-tuple of index_t representing \f$(i,j,k)\f$.
 ******************************************************************************/
template<typename G, typename E>
inline std::tuple<index_t, index_t, index_t> index_triple(const E e, const G& g)
{
  return std::tie(
      index(*(boost::in_edges(boost::source(e, g), g).first), g), index(e, g),
      index(*(boost::out_edges(boost::target(e, g), g).first), g));
}

/******************************************************************************
 * @brief Returns the index triple \f$(i,j,k) \in \tilde E\f$ defining an edge
 *        of a FaceDAG \f$\tilde G\f$ ,associated with the edge spanned by
 *        \f$(i,j)\f$  and \f$(j,k)\f$ represented by VertexDescs ij and jk.
 *
 * @attention index_triple(...) must be called on an intermediate edge,
 *            because the iterators returned by in_edges(...) and out_edges(...)
 *            are dereferenced without checking their validity.
 *
 * @param[in] ij VertexDesc of the edge whose index triple we want.
 * @param[in] g Const reference to the FaceDAG.
 * @returns 3-tuple of index_t representing \f$(i,j,k)\f$.
 ******************************************************************************/
template<typename G, typename V>
inline std::tuple<const index_t, const index_t, const index_t> index_triple(
    const V ij, const V jk, const G& g)
{
  return std::tie(
      index(*(boost::in_edges(ij, g).first), g),
      index(*(boost::out_edges(ij, g).first), g),
      index(*(boost::out_edges(jk, g).first), g));
}

/******************************************************************************
 * @brief Returns the index pair \f$(i,j) \in E\f$ associated
 *        with a vertex \f$v \in \tilde V\f$ of a FaceDAG.
 *
 * @attention index_pair(...) must be called on an
 *            intermediate vertex, because the iterators
 *            returned by in_edges(...) and out_edges(...)
 *            are dereferenced without checking their validity.
 *
 * @param[in] v Vertex descriptor of the vertex whose index pair we want.
 * @param[in] g Const reference to the FaceDAG.
 * @returns 2-tuple of index_t representing \f$(i,j)\f$.
 ******************************************************************************/
template<typename G, typename V>
inline std::tuple<index_t, index_t> index_pair(const V v, const G& g)
{
  return std::tie(
      index(*(boost::in_edges(v, g).first), g),
      index(*(boost::out_edges(v, g).first), g));
}

/******************************************************************************
 * @brief Accesses the set of edges leading into the source of an edge e.
 *
 * @param[in] e the edge.
 * @param[in] g the DAG.
 * @returns a pair of iterators for the in-edge set of the source of e.
 ******************************************************************************/
template<typename E, typename G>
inline auto lower_accessor::next_edges(E e, G& g)
{
  return boost::in_edges(boost::source(e, g), g);
}

/******************************************************************************
 * @brief Accesses the set of edges leading into a vertex v.
 *
 * @param[in] v the vertex.
 * @param[in] g the DAG.
 * @returns a pair of iterators for the in-edge set v.
 ******************************************************************************/
template<typename V, typename G>
inline auto lower_accessor::edges(V v, G& g)
{
  return boost::in_edges(v, g);
}

/******************************************************************************
 * @brief Accesses the in-degree \f$|P(v)|\f$ of a vertex v.
 *
 * @param[in] v the vertex.
 * @param[in] g the DAG.
 * @returns size_t the in-degree of v.
 ******************************************************************************/
template<typename V, typename G>
inline auto lower_accessor::degree(V v, G& g)
{
  return boost::in_degree(v, g);
}

/******************************************************************************
 * @brief Yields an abritrary edge from the in-edge set of the source of an
 *        edge e.
 *
 * @param[in] e the edge.
 * @param[in] g the DAG.
 * @returns the result of dereferencing the iterator pointing to the first
 *          element of the in-edge set.
 ******************************************************************************/
template<typename E, typename G>
inline auto lower_accessor::next_edge(E e, G& g)
{
  return *(boost::in_edges(boost::source(e, g), g).first);
}

/******************************************************************************
 * @brief Yields the source \f$i \in V\f$ of an edge \f$(i,j) \in E\f$.
 *
 * @param[in] e the edge.
 * @param[in] g the DAG.
 * @returns boost::source(e, g)
 ******************************************************************************/
template<typename E, typename G>
inline auto lower_accessor::next_vertex(E e, G& g)
{
  return boost::source(e, g);
}

template<typename E, typename G>
inline auto upper_accessor::next_edges(E e, G& g)
{
  return boost::out_edges(boost::target(e, g), g);
}

template<typename V, typename G>
inline auto upper_accessor::edges(V v, G& g)
{
  return boost::out_edges(v, g);
}

template<typename V, typename G>
inline auto upper_accessor::degree(V v, G& g)
{
  return boost::out_degree(v, g);
}

template<typename E, typename G>
inline auto upper_accessor::next_edge(E e, G& g)
{
  return *(boost::out_edges(boost::target(e, g), g).first);
}

template<typename E, typename G>
inline auto upper_accessor::next_vertex(E e, G& g)
{
  return boost::target(e, g);
}

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_GRAPH_IMPL_DAG_HPP_
