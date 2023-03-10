#ifndef ADM_INC_GRAPH_UTILS_HPP_
#define ADM_INC_GRAPH_UTILS_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "misc/doxygen.hpp"

#include <boost/graph/graph_traits.hpp>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(DagIo)

//! Computes the longest directed path (...,v) ending in v.
template<typename Graph>
ADM_STRONG_INLINE plength_t longest_reverse_path_from(
    Graph& g, typename boost::graph_traits<Graph>::vertex_descriptor v);

//! Computes the longest directed path (v,...) starting with v.
template<typename Graph>
ADM_STRONG_INLINE plength_t longest_path_from(
    const Graph& g, typename boost::graph_traits<Graph>::vertex_descriptor v);

//! Computes the longest directed path (...,v,...) including v.
template<typename Graph>
ADM_ALWAYS_INLINE plength_t longest_bidirectional_path_from(
    const Graph& g, typename boost::graph_traits<Graph>::vertex_descriptor v);

//! Computes the length of the longest directed path in a (face) DAG.
template<typename Graph>
ADM_ALWAYS_INLINE plength_t longest_path(const Graph& g);

DOXYGEN_MODULE_END(DagIo)

}  // end namespace admission

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "graph/impl/utils.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_GRAPH_UTILS_HPP_
