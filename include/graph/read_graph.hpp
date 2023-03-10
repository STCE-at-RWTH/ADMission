#ifndef ADM_INC_GRAPH_READ_GRAPH_HPP_
#define ADM_INC_GRAPH_READ_GRAPH_HPP_

// ################################ INCLUDES ################################ //

#include "graph/DAG.hpp"
#include "misc/doxygen.hpp"

#include <filesystem>
#include <fstream>
#include <stddef.h>
#include <stdexcept>
#include <string>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(DagIo)

/******************************************************************************
 * @brief Exception to be thrown if the dimensions of elemental
 *        Jacobians do not match.
 ******************************************************************************/
class matrix_dim_mismatch : public std::runtime_error
{
 public:
  //! Constructor of matrix_dim_mismatch.
  matrix_dim_mismatch(size_t a, size_t b, std::string msg = "!");
};

//! Reads a graph from the format used in Tamme Claus Seminar Paper.
void read_mmchain(std::ifstream& stream, admission::DAG& g);

//! Reads a DAG from a GRAPHML file.
void read_graphml(std::ifstream& stream, admission::DAG& g);

//! Reads a FaceDAG from a GRAPHML file.
void read_graphml(std::ifstream& stream, admission::FaceDAG& g);

//! Reads a DAG from different formats.
void read_graph(fs::path p, admission::DAG& g);

DOXYGEN_MODULE_END(DagIo)

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_GRAPH_READ_GRAPH_HPP_
