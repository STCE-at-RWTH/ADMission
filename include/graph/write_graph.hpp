#ifndef ADM_INC_GRAPH_WRITE_GRAPH_HPP_
#define ADM_INC_GRAPH_WRITE_GRAPH_HPP_

// ################################ INCLUDES ################################ //

#include "graph/DAG.hpp"
#include "misc/doxygen.hpp"

#include <iosfwd>
#include <string>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(DagIo)

namespace detail
{

//! Writes an admission::DAG to a file in GRAPHML format.
void write_graphml(std::ostream& os, admission::DAG& g);

//! Writes an admission::FaceDAG to a file in GRAPHML format.
void write_graphml(std::ostream& os, admission::FaceDAG& g);

}  // end namespace detail

//! Writes an admission::DAG to a file as mmchain.
void write_mmchain(std::ostream& os, admission::DAG& g);

//! Writes a DAG or a face DAG to a file in GRAPHML format.
template<typename Graph>
void write_graphml(std::string path, Graph& g);

DOXYGEN_MODULE_END(DagIo)

}  // end namespace admission

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "graph/impl/write_graph.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_GRAPH_WRITE_GRAPH_HPP_
