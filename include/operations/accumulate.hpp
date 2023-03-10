#ifndef ADM_INC_OPERATIONS_ACCUMULATE_HPP_
#define ADM_INC_OPERATIONS_ACCUMULATE_HPP_

// ################################ INCLUDES ################################ //

#include "graph/DAG.hpp"

#include <memory>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

//! Accumulates the Jacobian of a DAG g by simply multiplying along
//! all paths between all minimal \f$x \in X\f$ and all maximal
//! \f$y \in Y\f$ vertices and adding the Jacobians \f$F'_{x,y}\f$
//! of all paths.
std::shared_ptr<FaceDAG> accumulate_trivial(const DAG& g);

std::shared_ptr<FaceDAG> accumulate_trivial(const FaceDAG& g);

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_OPERATIONS_ACCUMULATE_HPP_
