#ifndef DAG_HPP
#define DAG_HPP

// ******************************** Includes ******************************** //

#include "admission_config.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_selectors.hpp>
#include <boost/graph/properties.hpp>
#include <boost/pending/property.hpp>

#include <stddef.h>
#include <tuple>

// ************************** Forward declarations ************************** //

namespace admission
{
struct upper_accessor;
}

// **************************** Header contents ***************************** //

/**
 * \defgroup Dag Definitions of containers and their properties
 *           to store DAGs, face DAGs and meta DAGs.
 * \addtogroup Dag
 * @{
 */

/**\brief Repetition of boost macro for jointly defining
 * a property and installing it.
 *
 * Calling the version contained in boost results in an error
 * for some reason. Boost's internal calls to it work, however.
 */
#define ADM_DEF_PROPERTY(KIND, NAME)                                           \
  enum KIND##_##NAME##_t{KIND##_##NAME};                                       \
  BOOST_INSTALL_PROPERTY(KIND, NAME);

namespace boost
{

// ------------------- Custom properties of DAG vertices -------------------- //

/// Size of a vertex i, denoted n(i) in formulas.
ADM_DEF_PROPERTY(vertex, size);

// ---------------- Custom properties of Face DAG vertices ------------------ //

/// Cost for an execution of the vector-adjoint model of \f$F_{ij}\f$, denoted  \f$c_{a}(ij)\f$ in formulas.
ADM_DEF_PROPERTY(vertex, c_adj);
/// Cost for an execution of the vector-tangent model of \f$F_{ij}\f$, denoted  \f$c_{t}(ij)\f$ in formulas.
ADM_DEF_PROPERTY(vertex, c_tan);
/// Indicates whether \f$F'_ij\f$ has been preaccumulated.
ADM_DEF_PROPERTY(vertex, acc_stat);
/// Indicates whether the models \f$\bar F_{ij} and \dot F_{ij}\f$ exist.
ADM_DEF_PROPERTY(vertex, has_model);

// -------------------- Custom properties of DAG edges ---------------------- //

/// Cost for an execution of the vector-adjoint model of \f$F_{ij}\f$, denoted \f$c_a(ij)\f$ in formulas.
ADM_DEF_PROPERTY(edge, c_adj);
/// Cost for an execution of the vector-tangent model of \f$F_{ij}\f$, denoted \f$c_t(ij)\f$ in formulas.
ADM_DEF_PROPERTY(edge, c_tan);
/// Indicates whether \f$F'_{ij}\f$ has been preaccumulated.
ADM_DEF_PROPERTY(edge, acc_stat);
/// Indicates whether the models \f$\bar F_{ij}\f$ and \f$\dot F_{ij}\f$ exist.
ADM_DEF_PROPERTY(edge, has_model);

// ------------------ Custom properties of Face DAG edges ------------------- //

/// Size of a vertex \f$j \in V\f$ corresponding to an edge \f$(i,j,k) \in \tilde E\f$, denoted \f$n(i)\f$.
ADM_DEF_PROPERTY(edge, size);

// ---------------- Custom properties of DAGs and FaceDAGs ------------------ //

/// Identifier of the elimination that was used to generate a face DAG.
ADM_DEF_PROPERTY(graph, previous_op);

// --------------------- Custom property for MetaDAGs ----------------------- //

/// Denotes the current optimal solution's cost.
ADM_DEF_PROPERTY(graph, opt);

// ------------------ Custom property of MetaDAG vertices ------------------- //

/// A hash to reference the face DAG corresponding to a vertex of a meta DAG.
ADM_DEF_PROPERTY(vertex, info);

}  // end namespace boost

namespace admission
{

/**\brief Shorthand for boost::property.
 */
template<typename P, typename T, typename N = boost::no_property>
using property = boost::property<P, T, N>;

/**Properties attached to a vertex
 * \f$j \in V\f$ of a DAG \f$G\f$.
 */
using DAGVertex = property<
    boost::vertex_index_t, index_t, property<boost::vertex_size_t, flop_t>>;

/**Properties attached to a vertex
 * \f$(i,j) \in \tilde V\f$ of a face DAG \f$\tilde G\f$.
 * Identical to the properties of an edge \f$e \in E\f$ of a DAG.
 */
using FaceDAGVertex = property<
    boost::vertex_index_t, index_t,
    property<
        boost::vertex_acc_stat_t, bool,
        property<
            boost::vertex_has_model_t, bool,
            property<
                boost::vertex_c_tan_t, flop_t,
                property<boost::vertex_c_adj_t, flop_t>>>>>;

/**Properties attached to a DAG edge
 * \f$e \in E\f$.
 */
using DAGEdge = property<
    boost::edge_index_t, index_t,
    property<
        boost::edge_acc_stat_t, bool,
        property<
            boost::edge_has_model_t, bool,
            property<
                boost::edge_c_tan_t, flop_t,
                property<boost::edge_c_adj_t, flop_t>>>>>;

/**\brief Properties attached to an edge
 * \f$e \in \tilde E\f$ of a face DAG.
 *
 * Corresponds to the properties of a
 * DAG vertex \f$v \in V\f$ but needs an index
 * that is implicitly defined as by the
 * VertexDesc of v.
 */
using FaceDAGEdge =
    property<boost::edge_index_t, index_t, property<boost::edge_size_t, flop_t>>;

/**\brief Property attached to a face DAG.
 */
using FaceDAGProperties =
    property<boost::graph_previous_op_t, std::tuple<index_t, index_t, index_t>>;

/** \brief Generic DAG structure with placeholder properties.
 *
 *  Uses a std::set to store the in- and out-edges of vertices.
 *  Uses a std::vector to store vertices.
 */
template<typename VertexP, typename EdgeP, typename GraphP>
using DAGBase = boost::adjacency_list<
    boost::setS, boost::vecS, boost::bidirectionalS, VertexP, EdgeP, GraphP>;

/**\brief The DAG class with respective properties attached
 *        to its vertices and edges.
 */
using DAG = DAGBase<DAGVertex, DAGEdge, boost::no_property>;

/**\brief The Face DAG class with respective properties attached
 *        to its vertices and edges.
 */
using FaceDAG = DAGBase<FaceDAGVertex, FaceDAGEdge, FaceDAGProperties>;

/**\brief Bundled property of a meta DAG vertex \f$v \in V_m\f$
 *
 *  Containing
 *  idx: The index of the corresponding face DAG.
 *  cut: Whether the branch spanned from this face DAG was cut
 *       by the branch and bound algorithm.
 *  was_opt: If this face DAG is a leaf, was it ever considerd
 *           to be an optimal solution?
 */
struct meta_dag_info_t
{
  size_t idx;
  bool cut;
  bool was_opt;
};

/**\brief The meta DAG class with respective properties
 *        for vertices and itself.
 */
using MetaDAG = boost::adjacency_list<
    boost::vecS, boost::vecS, boost::directedS,
    boost::property<boost::vertex_info_t, meta_dag_info_t>, boost::no_property,
    boost::property<boost::graph_opt_t, size_t>>;

/**\brief Structural traits of the DAG and FaceDAG classes. */
using GraphContTraits = boost::adjacency_list_traits<
    boost::setS, boost::vecS, boost::bidirectionalS>;

/**\brief VertexDesc which is equivalent to an unsigned integer. */
using VertexDesc = GraphContTraits::vertex_descriptor;

/**\brief EdgeDesc which is some obscure iterator over a list. */
using EdgeDesc = GraphContTraits::edge_descriptor;

/**\brief Wrapper for returning the index of an edge,
 *        relieving us from the boost::get syntax.
 *
 * Phew...
 */
template<typename G, typename E>
ADM_ALWAYS_INLINE const index_t& index(const E e, const G& g)
{
  return boost::get(boost::edge_index, g, e);
}

/**\brief Converts an EdgeDesc e of a FaceDAG g to the index triple
 *        \f$(i,j,k) \in \tilde E\f$ defining an edge of a FaceDAG \f$\tilde G\f$.
 *
 * \attention index_triple(...)must be called on an intermediate edge,
 * because the iterators returned by in_edges(...) and out_edges(...)
 * are dereferenced without checking their validity.
 *
 * @param[in] e EdgeDesc of the edge whose index triple we want.
 * @param[in] g Const reference to the FaceDAG.
 * @returns 3-tuple of index_t representing \f$(i,j,k)\f$.
 */
template<typename G, typename E>
inline std::tuple<index_t, index_t, index_t> index_triple(const E e, const G& g)
{
  return std::tie(
      index(*(boost::in_edges(boost::source(e, g), g).first), g), index(e, g),
      index(*(boost::out_edges(boost::target(e, g), g).first), g));
}

/**\brief Returns the index triple \f$(i,j,k) \in \tilde E\f$ defining an edge
 *        of a FaceDAG \f$\tilde G\f$ ,associated with the edge spanned by
 *        \f$(i,j)\f$  and \f$(j,k)\f$ represented by VertexDescs ij and jk.
 *
 * \attention index_triple(...) must be called on an intermediate edge,
 * because the iterators returned by in_edges(...) and out_edges(...)
 * are dereferenced without checking their validity.
 *
 * @param[in] ij VertexDesc of the edge whose index triple we want.
 * @param[in] g Const reference to the FaceDAG.
 * @returns 3-tuple of index_t representing \f$(i,j,k)\f$.
 */
template<typename G, typename V>
inline std::tuple<const index_t, const index_t, const index_t> index_triple(
    const V ij, const V jk, const G& g)
{
  return std::tie(
      index(*(boost::in_edges(ij, g).first), g),
      index(*(boost::out_edges(ij, g).first), g),
      index(*(boost::out_edges(jk, g).first), g));
}

/**\brief Returns the index pair \f$(i,j) \in E\f$ associated
 *        with a vertex \f$v \in \tilde V\f$ of a FaceDAG.
 *
 * \attention index_pair(...) must be called on an
 * intermediate vertex, because the iterators
 * returned by in_edges(...) and out_edges(...)
 * are dereferenced without checking their validity.
 *
 * @param[in] v Vertex descriptor of the vertex whose index pair we want.
 * @param[in] g Const reference to the FaceDAG.
 * @returns 2-tuple of index_t representing \f$(i,j)\f$.
 */
template<typename G, typename V>
inline std::tuple<index_t, index_t> index_pair(const V v, const G& g)
{
  return std::tie(
      index(*(boost::in_edges(v, g).first), g),
      index(*(boost::out_edges(v, g).first), g));
}

/**\brief Forward-declared lower_accessor to ease building algorithms
 *        on DAGs that can be applied symmetrically.
 *
 * Forward-declaration is neccesary to make it
 * accessible as the reverse of the \ref upper_accessor.
 * Definition of the lower_accessor that contains
 * a set of static member functions (making them
 * callable from the type, not just from an object)
 * that give access to the boost::in_edges(...)
 * boost::source(...) etc. functions.
 */
struct lower_accessor
{
  /// \ref upper_accessor is the reverse.
  typedef upper_accessor reverse;

  /**Accesses the set of edges leading
   * into the source of an edge e.
   * @param[in] e the edge.
   * @param[in] g the DAG.
   * @returns a pair of iterators for
   * the in-edge set of the source of e.
   */
  template<typename E, typename G>
  inline static auto next_edges(E e, G& g)
  {
    return boost::in_edges(boost::source(e, g), g);
  }

  /**Accesses the set of edges leading
   * into a vertex v.
   * @param[in] v the vertex.
   * @param[in] g the DAG.
   * @returns a pair of iterators
   * for the in-edge set v.
   */
  template<typename V, typename G>
  inline static auto edges(V v, G& g)
  {
    return boost::in_edges(v, g);
  }

  /**Accesses the in-degree \f$|P(v)|\f$
   * of a vertex v.
   * @param[in] v the vertex.
   * @param[in] g the DAG.
   * @returns size_t the in-degree of v.
   */
  template<typename V, typename G>
  inline static auto degree(V v, G& g)
  {
    return boost::in_degree(v, g);
  }

  /**Yields an abritrary edge from the
   * in-edge set of the source of an
   * edge e.
   * @param[in] e the edge.
   * @param[in] g the DAG.
   * @returns the result of dereferencing
   * the iterator pointing to the first
   * element of the in-edge set.
   */
  template<typename E, typename G>
  inline static auto next_edge(E e, G& g)
  {
    return *(boost::in_edges(boost::source(e, g), g).first);
  }

  /**Yields the source \f$i \in V\f$
   * of an edge \f$(i,j) \in E\f$.
   * @param[in] e the edge.
   * @param[in] g the DAG.
   * @returns boost::source(e, g)
   */
  template<typename E, typename G>
  inline static auto next_vertex(E e, G& g)
  {
    return boost::source(e, g);
  }
};

/**\brief Forward-declared upper_accessor to be
 *         accessible from the \ref lower_accessor.
 *
 *Definition of the upper_accessor that contains
 * a set of static member functions (making them
 * callable from the type, not just from an object)
 * that give access to the boost::in_edges(...)
 * boost::source(...) etc. functions.
 * \note The upper_accessor declares the same static
 * member functions as the lower_accesor. See the
 * respective declaration.
 */
struct upper_accessor
{
  /// \ref lower_accessor is the reverse.
  typedef lower_accessor reverse;

  ///\name See according declarations in lower_accessor.
  ///@{
  template<typename E, typename G>
  inline static auto next_edges(E e, G& g)
  {
    return boost::out_edges(boost::target(e, g), g);
  }

  template<typename V, typename G>
  inline static auto edges(V v, G& g)
  {
    return boost::out_edges(v, g);
  }

  template<typename V, typename G>
  inline static auto degree(V v, G& g)
  {
    return boost::out_degree(v, g);
  }

  template<typename E, typename G>
  inline static auto next_edge(E e, G& g)
  {
    return *(boost::out_edges(boost::target(e, g), g).first);
  }

  template<typename E, typename G>
  inline static auto next_vertex(E e, G& g)
  {
    return boost::target(e, g);
  }

  ///@}
};

}  // end namespace admission

/**
 * @}
 */

#endif  //DAG_HPP
