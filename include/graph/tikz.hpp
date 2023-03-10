#ifndef ADM_INC_GRAPH_TIKZ_HPP_
#define ADM_INC_GRAPH_TIKZ_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "misc/doxygen.hpp"

#include <filesystem>
#include <fstream>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(DagIo)

//! Function to print a DAG to a text-file in TikZ.
void write_tikz(std::ostream& out, const DAG& g);

//! Function to print a FaceDAG to a text-file in TikZ.
void write_tikz(std::ostream& out, const FaceDAG& g);

//! Function to print a MetaDAG to a text-file in TikZ.
void write_tikz(std::ostream& out, const MetaDAG& g);

//! Wrapper for writing DAGs and face DAGs to a file specified by a
//! path instead of an ostream.
template<typename Graph>
ADM_ALWAYS_INLINE void write_tikz(fs::path p, const Graph& g);

DOXYGEN_MODULE_END(DagIo)

}  // end namespace admission

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "graph/impl/tikz.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_GRAPH_TIKZ_HPP_
