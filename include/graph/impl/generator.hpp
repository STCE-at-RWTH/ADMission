#ifndef ADM_INC_GRAPH_IMPL_GENERATOR_HPP_
#define ADM_INC_GRAPH_IMPL_GENERATOR_HPP_

// ################################ INCLUDES ################################ //

#include "graph/generator.hpp"  // IWYU pragma: keep

#include <cmath>

// ###################### TEMPLATE AND INLINE CONTENTS ###################### //

namespace admission
{

/******************************************************************************
 * @brief Generates a weighted discrete distribution.
 *
 * Generates a weighted discrete distribution with
 * randomly generated values within the range [1, l].
 *
 * @tparam T underlying numeric type of the distribution.
 * @param[in] l lenght of the range.
 * @returns the weighted distribution.
 ******************************************************************************/
template<typename Graph>
template<typename T>
T GraphGenerator<Graph>::dist(T l)
{
  return weighted_dist_t<T>(
      l, 0, l,
      [&](double x)
      {
        return _p.coeff_const + _p.coeff_ascending * x / l +
               _p.coeff_descending * (l - x) / l +
               _p.coeff_sin *
                   sin(std::sqrt((x / static_cast<double>(l))) * 3.142);
      })(_gen);
}

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_GRAPH_IMPL_GENERATOR_HPP_
