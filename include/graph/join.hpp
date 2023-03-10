#ifndef ADM_INC_GRAPH_JOIN_HPP_
#define ADM_INC_GRAPH_JOIN_HPP_

// ################################ INCLUDES ################################ //

#include "graph/DAG.hpp"

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

//! Joins vertices of a DAG that have identical predecessor and successor sets.
void join_vertices(DAG& g);

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_GRAPH_JOIN_HPP_
