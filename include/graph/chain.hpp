#ifndef ADM_INC_GRAPH_CHAIN_HPP_
#define ADM_INC_GRAPH_CHAIN_HPP_

// ################################ INCLUDES ################################ //

#include "graph/DAG.hpp"
#include "misc/doxygen.hpp"

#include <memory>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(DagCreate)

//! Constructs a Jacobian chain from a DAG.
std::shared_ptr<admission::DAG> make_chain(admission::DAG& g);

DOXYGEN_MODULE_END(DagCreate)

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_GRAPH_CHAIN_HPP_
