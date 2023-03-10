#ifndef ADM_INC_GRAPH_MIN_MAX_HPP_
#define ADM_INC_GRAPH_MIN_MAX_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "misc/doxygen.hpp"

#include <boost/graph/graph_traits.hpp>

#include <iterator>
#include <list>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(DagInfo)

//! Returns a list containing the minimal vertices.
template<typename Graph>
ADM_ALWAYS_INLINE std::list<
    typename boost::graph_traits<Graph>::vertex_descriptor>
min_vertices(const Graph& g);

//! Returns a list of vertex_descriptors which are maximal vertices.
template<typename Graph>
ADM_ALWAYS_INLINE std::list<
    typename boost::graph_traits<Graph>::vertex_descriptor>
max_vertices(const Graph& g);

//! Returns a list of vertex_descriptors of the face graph g which correspond
//! to the non-intermediate edges in the graph from which g was obtained.
template<typename Graph>
ADM_ALWAYS_INLINE std::list<
    std::pair<typename boost::graph_traits<Graph>::vertex_descriptor, bool>>
corr_non_inter_edges(const Graph& g);

//! Returns a list of vertex_descriptors of the face graph g which correspond
//! to the intermediate edges in the graph from which g was obtained.
template<typename Graph>
ADM_ALWAYS_INLINE std::list<
    typename boost::graph_traits<Graph>::vertex_descriptor>
corr_inter_edges(const Graph& g);

DOXYGEN_MODULE_END(DagInfo)

}  // end namespace admission

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "graph/impl/min_max.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_GRAPH_MIN_MAX_HPP_
