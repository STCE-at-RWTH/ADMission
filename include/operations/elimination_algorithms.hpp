#ifndef ADM_INC_OPERATIONS_ELIMINATION_ALGORITHMS_HPP_
#define ADM_INC_OPERATIONS_ELIMINATION_ALGORITHMS_HPP_

// ################################ INCLUDES ################################ //

#include "graph/DAG.hpp"
#include "misc/doxygen.hpp"

#include <tuple>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(Algorithms)

//! Checks if a vertex that is part of an elimination will (can) be merged.
template<typename Acc>
std::tuple<bool, bool, VertexDesc> check_merge(
    const VertexDesc to_merge, const VertexDesc to_ignore, const FaceDAG& g);

//! (Rule 2)(1.) Checks if the the resulting vertex from eliminating (i,j,k)
//! can be absorbed into another vertex (i,*,k).
std::tuple<bool, bool, VertexDesc> check_absorption(
    const VertexDesc ij, const VertexDesc jk, const FaceDAG& g);

//! Eliminates an edge \f$(i,j,k)\f$ from the face DAG g according
//!  to the generalised face-elimination algorithm (Rule 2).
void eliminate_edge(const EdgeDesc ijk, FaceDAG& g);

//! Eliminates an edge \f$(i,j,k)\f$ from the face DAG g according
//! to the generalised face-elimination algorithm (Rule 2).
void eliminate_edge(const VertexDesc ij, const VertexDesc jk, FaceDAG& g);

//! Performs a preaccumulation as used in the naive face
//! elimination algorithm according to (Rule 3).
void preaccumulate(
    const VertexDesc ij, FaceDAG& g, const bool remove_models = false);

//! Converts a DAG to have preaccumulated elemental Jacobians and no tangent
//! or adjoint models.
void preaccumulate_all(FaceDAG& g, bool remove_models = false);

DOXYGEN_MODULE_END(Algorithms)

}  // end namespace admission

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "operations/impl/elimination_algorithms.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_OPERATIONS_ELIMINATION_ALGORITHMS_HPP_
