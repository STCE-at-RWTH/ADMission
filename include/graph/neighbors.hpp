#ifndef ADM_INC_GRAPH_NEIGHBORS_HPP_
#define ADM_INC_GRAPH_NEIGHBORS_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "misc/doxygen.hpp"

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(DagInfo)

// --------------------------- Vertex relations ----------------------------- //

//! Counts the number m of shared neighbors of two vertices \f$a,b \in V\f$.
template<typename G, typename Dir>
plength_t num_shared_neighbors(
    const G& g, const VertexDesc a, const VertexDesc b);

//! Returns true, if the neighbor sets \f$N(a)\f$ and \f$N(b)\f$
//! of vertices a and b are identical.
template<typename G, typename Dir>
ADM_ALWAYS_INLINE bool same_neighbors(
    const G& g, const VertexDesc a, const VertexDesc b);

//! Returns true, if the neighbor sets \f$N(a)\f$ and \f$N(b)\f$
//! of vertices a and b are identical except one vertex.
template<typename Dir>
bool same_neighbors_but_one(
    const FaceDAG& g, const VertexDesc a, const VertexDesc b,
    const VertexDesc c);

//! Returns true, if the neighbor sets of vertices a and b overlap i.e.
//! \f$N(a) \cup N(b) \neq N(a) \cap N(b) \neq 0\f$.
template<typename Dir>
ADM_ALWAYS_INLINE bool share_few_neighbors(
    const FaceDAG& g, const VertexDesc a, const VertexDesc b);

// --------------------------- Function aliases ----------------------------- //

/******************************************************************************
 * @brief Explicit instantiation of \ref same_neighbors() to compare
 *        predecessor sets \f$|P(i)|\f$ and \f$|P(j)|\f$.
 ******************************************************************************/
const auto same_predecessors = same_neighbors<FaceDAG, lower_accessor>;

/******************************************************************************
 * @brief Explicit instantiation of \ref same_neighbors() to compare
 *        successor sets \f$|P(i)|\f$ and \f$|P(j)|\f$.
 ******************************************************************************/
const auto same_successors = same_neighbors<FaceDAG, upper_accessor>;

/******************************************************************************
 * @brief Explicit instantiation of \ref same_neighbors_but_one() to compare
 *        predecessor sets \f$|P(i)|\f$ and \f$|P(j)|\f$.
 ******************************************************************************/
const auto same_predecessors_but_one = same_neighbors_but_one<lower_accessor>;

/******************************************************************************
 * @brief Explicit instantiation of \ref same_neighbors_but_one() to compare
 *        successor sets \f$|P(i)|\f$ and \f$|P(j)|\f$.
 ******************************************************************************/
const auto same_successors_but_one = same_neighbors_but_one<upper_accessor>;

/******************************************************************************
 * @brief Explicit instantiation of \ref share_few_neighbors() to compare
 *        successor sets \f$|P(i)|\f$ and \f$|P(j)|\f$.
 ******************************************************************************/
const auto share_few_predecessors = share_few_neighbors<lower_accessor>;

/******************************************************************************
 * @brief Explicit instantiation of \ref share_few_neighbors() to compare
 *        successor sets \f$|P(i)|\f$ and \f$|P(j)|\f$.
 ******************************************************************************/
const auto share_few_successors = share_few_neighbors<upper_accessor>;

DOXYGEN_MODULE_END(DagInfo)

}  // end namespace admission

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "graph/impl/neighbors.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_GRAPH_NEIGHBORS_HPP_
