#ifndef TIKZ_HPP
#define TIKZ_HPP

// ******************************** Includes ******************************** //

#include "graph/DAG.hpp"

#include <filesystem>
#include <fstream>

// **************************** Header contents ***************************** //

namespace admission
{

/**
 * \addtogroup DagIo
 * @{
 */

/**\brief Function to print a DAG to a text-file in TikZ.
 *
 * @param[inout] out Reference to a std::ostream to which the tikz code is written.
 * @param[in] g const reference to the graph we write.
 */
void write_tikz(std::ostream& out, const DAG& g);

/**\brief Function to print a FaceDAG to a text-file in TikZ.
 *
 * @param[inout] out Reference to a std::ostream to which the tikz code is written.
 * @param[in] g const reference to the graph we write.
 */
void write_tikz(std::ostream& out, const FaceDAG& g);

/**\brief Function to print a MetaDAG to a text-file in TikZ.
 *
 * @param[inout] out Reference to a std::ostream to which the tikz code is written.
 * @param[in] g const reference to the graph we write.
 */
void write_tikz(std::ostream& out, const MetaDAG& g);

/**\brief Wrapper for writing DAGs and face DAGs
 * to a file specified by a path instead
 * of an ostream.
 *
 * @tparam Graph Type of the DAD to write.
 * @param[in] p path object (constructible from std::string).
 * @param[in] g Const ref to the DAG or face DAG.
 */
template<typename Graph>
inline void write_tikz(fs::path p, const Graph& g)
{
  std::ofstream o(p);
  write_tikz(o, g);
  o.close();
}

/**
 * @}
 */

}  // end namespace admission

#endif  // TIKZ_HPP
