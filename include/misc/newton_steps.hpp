#ifndef ADM_INC_MISC_NEWTON_STEPS_HPP_
#define ADM_INC_MISC_NEWTON_STEPS_HPP_

// ################################ INCLUDES ################################ //

#include "graph/DAG.hpp"

#include <memory>

// ############################ SOURCE CONTENTS ############################# //

namespace admission
{

//! From a single-newton-step-DAG creates a multi-newton-step-DAG.
std::shared_ptr<DAG> make_newton_steps(DAG& n1, unsigned int n);

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_MISC_NEWTON_STEPS_HPP_
