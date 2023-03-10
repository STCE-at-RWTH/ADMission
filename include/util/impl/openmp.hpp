#ifndef ADM_INC_UTIL_IMPL_OPENMP_HPP_
#define ADM_INC_UTIL_IMPL_OPENMP_HPP_

// ################################ INCLUDES ################################ //

#include "util/openmp.hpp"

#ifdef _OPENMP
#include <omp.h>
#else
#include <chrono>
#endif

// ###################### TEMPLATE AND INLINE CONTENTS ###################### //

namespace admission
{

/******************************************************************************
 * @brief Get the maximum amount of threads.
 *
 * @returns The maximum number of OpenMP threads if we compile with -fopenmp.
 *          Otherwise we return 1.
 ******************************************************************************/
ADM_ALWAYS_INLINE size_t get_max_threads()
{
#ifdef _OPENMP
  return static_cast<size_t>(omp_get_max_threads());
#else
  return static_cast<size_t>(1);
#endif
}

/******************************************************************************
 * @brief Get the number of threads.
 *
 * @returns The current number of OpenMP threads if we compile with -fopenmp.
 *          Otherwise we return 1.
 ******************************************************************************/
ADM_ALWAYS_INLINE size_t get_num_threads()
{
#ifdef _OPENMP
  return static_cast<size_t>(omp_get_num_threads());
#else
  return static_cast<size_t>(1);
#endif
}

/******************************************************************************
 * @brief Get the number of the current thread.
 *
 * @returns The number of the current OpenMP thread if we compile with -fopenmp.
 *          Otherwise we return 0.
 ******************************************************************************/
ADM_ALWAYS_INLINE size_t get_thread_num()
{
#ifdef _OPENMP
  return static_cast<size_t>(omp_get_thread_num());
#else
  return static_cast<size_t>(0);
#endif
}

/******************************************************************************
 * @brief Get the elapsed wall clock time in seconds.
 *
 * @returns The number of seconds elapsed since some "time in the past".
 ******************************************************************************/
ADM_ALWAYS_INLINE double get_wtime()
{
#ifdef _OPENMP
  return omp_get_wtime();
#else
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
             .count() /
         1000.0;
#endif
}

/******************************************************************************
 * @brief Set the number of threads.
 *
 * @param[in] The number of OpenMP threads.
 ******************************************************************************/
ADM_ALWAYS_INLINE void set_num_threads(size_t n)
{
#ifdef _OPENMP
  omp_set_num_threads(n);
#else
  (void) n;
#endif
}

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_UTIL_IMPL_OPENMP_HPP_
