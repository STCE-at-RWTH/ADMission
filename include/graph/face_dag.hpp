#ifndef ADM_INC_GRAPH_FACE_DAG_HPP_
#define ADM_INC_GRAPH_FACE_DAG_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "misc/doxygen.hpp"

#include <memory>
#include <utility>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(DagCreate)

//! Finds the global mininmal vertex index inside a DAG.
template<typename Graph>
ADM_ALWAYS_INLINE index_t min_vertex_index(const Graph& g);

//! Finds the global maximal vertex index inside a DAG.
template<typename Graph>
ADM_ALWAYS_INLINE index_t max_vertex_index(const Graph& g);

//! Adds a vertex to a graph and initialises it with the
//! corresponding edge from the source DAG.
ADM_ALWAYS_INLINE VertexDesc
add_vertex_from(DAG& g, const EdgeDesc e, FaceDAG& g_face);

//! Adds an edge to a FaceDAG and initialises it with the
//! corresponding vertex from the DAG.
ADM_ALWAYS_INLINE std::pair<EdgeDesc, bool> add_edge_from(
    const DAG& g, const VertexDesc v, FaceDAG& g_face, const VertexDesc i,
    const VertexDesc j);

//! Constructs a face DAG from a DAG according to the Rule in (Definition 3).
std::shared_ptr<FaceDAG> make_face_dag(const DAG& g_const);

DOXYGEN_MODULE_END(DagCreate)

}  // end namespace admission

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "graph/impl/face_dag.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_GRAPH_FACE_DAG_HPP_
