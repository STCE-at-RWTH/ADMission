#ifndef GRAPH_JOIN_HPP
#define GRAPH_JOIN_HPP

// ******************************** Includes ******************************** //

#include "graph/DAG.hpp"

// **************************** Header contents ***************************** //

namespace admission
{

/**\brief Joins vertices of a DAG that have
 * identical predecessor and successor sets.
 */
void join_vertices(DAG& g);

}  // end namespace admission

#endif  // GRAPH_JOIN_HPP
