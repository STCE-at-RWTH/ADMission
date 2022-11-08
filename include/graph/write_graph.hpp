#ifndef WRITE_GRAPH_HPP
#define WRITE_GRAPH_HPP

// ******************************** Includes ******************************** //

#include "graph/DAG.hpp"

#include <iosfwd>
#include <string>

// **************************** Header contents ***************************** //

namespace admission
{

/**
 * \addtogroup DagIo
 * @{
 */

namespace detail
{

/**\brief Writes an admission::DAG
 * to a file in GRAPHML format.
 *
 * @param[inout] os Reference to the ostream to write to.
 * @param[in] g Reference to the DAG.
 */
void write_graphml(std::ostream& os, admission::DAG& g);

/**\brief Writes an admission::FaceDAG
 * to a file in GRAPHML format.
 *
 * @param[inout] os Reference to the ostream to write to.
 * @param[in] g Reference to the DAG.
 */
void write_graphml(std::ostream& os, admission::FaceDAG& g);

}  // end namespace detail

/**\brief Writes a DAG or a face DAG to
 * a file in GRAPHML format.
 *
 * @tparam Graph the type of the (face) DAG.
 * @param[in] path fs::path to the file we write to.
 * @param[in] g Reference to the DAG.
 */
template<typename Graph>
void write_graphml(std::string path, Graph& g);

/**
 * @}
 */

}  // end namespace admission

#endif  // WRITE_GRAPH_HPP
