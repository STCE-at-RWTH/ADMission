#ifndef ADM_INC_GRAPH_IMPL_WRITE_GRAPH_HPP_
#define ADM_INC_GRAPH_IMPL_WRITE_GRAPH_HPP_

// ################################ INCLUDES ################################ //

#include "graph/write_graph.hpp"  // IWYU pragma: keep

#include <fstream>  // IWYU pragma: keep
#include <string>

// ###################### TEMPLATE AND INLINE CONTENTS ###################### //

namespace admission
{

/******************************************************************************
 * @brief Writes a DAG or a face DAG to a file in GRAPHML format.
 *
 * @tparam Graph The type of the (face) DAG.
 * @param[in] path Path to the file we write to.
 * @param[in] g Reference to the DAG.
 ******************************************************************************/
template<typename Graph>
void write_graphml(std::string path, Graph& g)
{
  std::ofstream os(path, std::ofstream::out);
  admission::detail::write_graphml(os, g);
  os.close();
}

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_GRAPH_IMPL_WRITE_GRAPH_HPP_
