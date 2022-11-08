#ifndef READ_GRAPH_HPP
#define READ_GRAPH_HPP

// ******************************** Includes ******************************** //

#include "graph/DAG.hpp"

#include <filesystem>
#include <fstream>
#include <stddef.h>
#include <stdexcept>
#include <string>

// **************************** Header contents ***************************** //

namespace admission
{

/** \defgroup DagIo Reading and writing DAGs and FaceDAGs from and to files.
 *  \addtogroup DagIo
 *  @{
 */

/**\brief Exception to be thrown if the dimensions of elemental Jacobians do not match.
 */
class matrix_dim_mismatch : public std::runtime_error
{
 public:
  matrix_dim_mismatch(size_t a, size_t b, std::string msg = "!")
      : std::runtime_error(
            "Elemental Jacobian sizes " + std::to_string(a) + " and " +
            std::to_string(b) + " do not match" + msg)
  {}
};

/**\brief reads a graph from the format used in Tamme Claus Seminar Paper.
 * Reads graph from a single file used for optimal matrix product chaining.
 * @param[in]    stream Reference to an ifstream to read data from
 * @param[inout] g Reference to the graph that will represent the problem specified by the file.\ Its expected to be empty.
 */
void read_mmchain(std::ifstream& stream, admission::DAG& g);

/**\brief Reads a DAG from a GRAPHML file.
 *
 * Links all properties of an admission::DAG
 * to the dynamic properties of the
 * boost XML parser and delegates
 * to boost::read_graphml(...)
 * @param[in] stream Reference to an ifstream to read data from
 * @param[out] g Reference to the graph that will represent the problem specified by the file.\ Its expected to be empty.
 */
void read_graphml(std::ifstream& stream, admission::DAG& g);

/**\brief Reads a FaceDAG from a GRAPHML file.
 *
 * Links all properties of an admission::FaceDAG
 * to the dynamic properties of the
 * boost XML parser and delegates
 * to boost::read_graphml(...)
 * @param[in] stream Reference to an ifstream to read data from
 * @param[out] g Reference to the graph that will represent the problem specified by the file.\ Its expected to be empty.
 */
void read_graphml(std::ifstream& stream, admission::FaceDAG& g);

/**
 * \brief Reads a DAG from different formats.
 *
 * @param[in]  p Path to the file as a string
 * @param[out] g Reference to the Graph to be filled. If the graph is not empty, behaviour is undefined.
 * @throws std::runtime_error In case the file does not exist or is invalid
 * @throws boost::wrapexcept<boost::property_tree::xml_parser::xml_parser_error> in case the GRAPHML file is invalid
 * @throws admission::matrix_dim_mismatch in case of invalid dimensions.
 */
void read_graph(fs::path p, admission::DAG& g);

/**
 * @}
 */

}  // end namespace admission

#endif  // READ_GRAPH_HPP
