#ifndef ADM_INC_UTIL_OPENMP_HPP_
#define ADM_INC_UTIL_OPENMP_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"

#include <stddef.h>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

//! Get the maximum amount of threads.
ADM_ALWAYS_INLINE size_t get_max_threads();

//! Get the number of threads.
ADM_ALWAYS_INLINE size_t get_num_threads();

//! Get the number of the current thread.
ADM_ALWAYS_INLINE size_t get_thread_num();

//! Get the elapsed wall clock time in seconds.
ADM_ALWAYS_INLINE double get_wtime();

//! Set the number of threads.
ADM_ALWAYS_INLINE void set_num_threads(size_t n);

}  // end namespace admission

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "util/impl/openmp.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_UTIL_OPENMP_HPP_
