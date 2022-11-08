#ifndef FACE_DAG_HPP
#define FACE_DAG_HPP

// ******************************** Includes ******************************** //

#include "graph/DAG.hpp"

#include <memory>

// **************************** Header contents ***************************** //

namespace admission
{

/**
 * \addtogroup DagCreate Creating and transforming DAGs.
 * @{
 */

/**\brief Constructs a face DAG from a DAG according to the Rule in (Definition 3).
 *
 * @param[in] g_const Const reference to a DAG.
 * @returns std::shared_ptr to the face DAG corresponding to g_const.
 */
std::shared_ptr<FaceDAG> make_face_dag(const DAG& g_const);

/**
 * @}
 */

}  // end namespace admission

#endif  // FACE_DAG_HPP
