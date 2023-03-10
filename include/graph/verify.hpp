#ifndef ADM_INC_VERIFY_HPP_
#define ADM_INC_VERIFY_HPP_

// ################################ INCLUDES ################################ //

#include "graph/DAG.hpp"

#include <string>
#include <utility>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

//! Verifies that two face DAGs \f$a = (\tilde V_a, \tilde E_a)\f$ and
//! \f$b = (\tilde V_b, \tilde E_b)\f$ are equal.
std::pair<bool, std::string> verify_equal_DAGs(
    admission::FaceDAG& a, admission::FaceDAG& b);

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_VERIFY_HPP_
