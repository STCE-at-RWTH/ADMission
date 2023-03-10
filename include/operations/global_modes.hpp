#ifndef ADM_INC_GLOBAL_MODES_HPP_
#define ADM_INC_GLOBAL_MODES_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "misc/doxygen.hpp"
#include "operations/op_sequence.hpp"

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(DagInfo)

//! Computes the cost of global tangent mode on a dense FaceDAG.
flop_t global_tangent_cost(const FaceDAG& g);

//! Computes the cost of global adjoint mode on a dense FaceDAG.
flop_t global_adjoint_cost(const FaceDAG& g);

//! Computes the cost of global optimal preaccumulation of all
//! intermediate Jacobians.
OpSequence global_preaccumulation_ops(const FaceDAG& g);

DOXYGEN_MODULE_END(DagInfo)

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_GLOBAL_MODES_HPP_
