#ifndef ADM_INC_OPERATIONS_FIND_ELIMINATIONS_HPP_
#define ADM_INC_OPERATIONS_FIND_ELIMINATIONS_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "misc/doxygen.hpp"
#include "operations/op_sequence.hpp"

#include <utility>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(Algorithms)

// ---------------------------- Vertex merging ------------------------------ //

//! Checks if an unaccumulated vertex has an accumulated merge candidate
//! for a forced merge.
std::pair<bool, VertexDesc> has_merge_candidate(
    const VertexDesc ij, const FaceDAG& g);

//! Check if there are vertices that are mergeable by preaccumulation
//! cheaper than propagating their neighboring Jacobians through them, and
//! return the first that is found.
std::pair<OpSequence, VertexDesc> get_mergeable_vertex_on_any_graph(
    const admission::FaceDAG& g);

// ------------------------ Eliminations (inline) --------------------------- //

//! Creates an OpSequence that contains a tangent propagation through (i,j,k).
ADM_ALWAYS_INLINE OpSequence tan_prop(const EdgeDesc ijk, const FaceDAG& g);

//! Creates an OpSequence that contains an adjoint propagation through (i,j,k).
ADM_ALWAYS_INLINE OpSequence adj_prop(const EdgeDesc ijk, const FaceDAG& g);

//! Creates an OpSequence that contains an elimination of (i,j,k) by
//! matrix-matrix multiplication.
ADM_ALWAYS_INLINE OpSequence mul(const EdgeDesc ijk, const FaceDAG& g);

//! Returns the elimination Sequence that accumulates vertex v
//! for the cheapest cost.
ADM_ALWAYS_INLINE OpSequence
cheapest_preacc(const VertexDesc ij, const FaceDAG& g);

//! Returns the elimination Sequence that accumulates vertex v in tangent mode.
ADM_ALWAYS_INLINE OpSequence tan_preacc(const VertexDesc ij, const FaceDAG& g);

//! Returns the elimination Sequence that accumulates vertex v in adjoint mode.
ADM_ALWAYS_INLINE OpSequence adj_preacc(const VertexDesc ij, const FaceDAG& g);

// ----------------------------- Eliminations ------------------------------- //

//! Finds the cheapest way to eliminate an edge \f$(i,j,k)\f$.
OpSequence get_cheapest_elim(const EdgeDesc ijk, const FaceDAG& g);

//! Finds the cheapest elimination on a graph.
OpSequence get_cheapest_elim_on_any_graph(const FaceDAG& g);

//! Find the cost of global tangent mode and return the corresponding operation.
OpSequence glob_tan(const FaceDAG& g);

//! Find the cost of global adjoint mode and return the corresponding operation.
OpSequence glob_adj(const FaceDAG& g);

DOXYGEN_MODULE_END(Algorithms)

}  // end namespace admission

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "operations/impl/find_eliminations.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_OPERATIONS_FIND_ELIMINATIONS_HPP_
