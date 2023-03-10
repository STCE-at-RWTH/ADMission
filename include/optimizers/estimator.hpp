#ifndef ADM_INC_OPTIMIZERS_ESTIMATOR_HPP_
#define ADM_INC_OPTIMIZERS_ESTIMATOR_HPP_

// ################################ INCLUDES ################################ //

#include "graph/DAG.hpp"
#include "misc/doxygen.hpp"

#include <stddef.h>
#include <vector>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(Optimizers)

/******************************************************************************
 * @brief Struct to estimate the size of a branch and bound search space.
 ******************************************************************************/
class Estimator
{

 public:
  //! Default constructor of Estimator.
  Estimator() = default;

  //! Default destructor of Estimator.
  ~Estimator() = default;

  //! Initializes the estimations vector depending on the graph.
  void init(const FaceDAG& g);

  //! @todo
  double calc_est(const size_t d);

  //! @todo
  void add_cut(const size_t d);

  //! @todo
  void add_sample(const size_t d, const double& v);

 private:
  /****************************************************************************
   * @brief Estimate data struct.
   ****************************************************************************/
  struct Data
  {
    //! Number of samples.
    size_t n_samples = 0;

    //! Estimate of the search space size.
    double estimate = 1;
  };

  //! Vector of estimates.
  std::vector<Data> _estimates;
};

DOXYGEN_MODULE_END(Optimizers)

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_OPTIMIZERS_ESTIMATOR_HPP_
