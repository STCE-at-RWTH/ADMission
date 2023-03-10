// ################################ INCLUDES ################################ //

#include "optimizers/estimator.hpp"

#include "admission_config.hpp"

#include <boost/graph/adjacency_list.hpp>

#include <iostream>
#include <memory>

// ############################ SOURCE CONTENTS ############################# //

namespace admission
{

// ------------------------------- Estimator -------------------------------- //

/******************************************************************************
 * @brief Initializes the estimations vector depending on the graph.
 *
 * @param[in] g FaceDag& The input graph.
 ******************************************************************************/
void Estimator::init(const FaceDAG& g)
{
  _estimates.reserve(2 * num_edges(g) + num_vertices(g));
  _estimates.resize(0);
}

/******************************************************************************
 * @brief @todo.
 *
 * @param[in] d @todo.
 ******************************************************************************/
double Estimator::calc_est(const size_t d)
{
  double est = 1.0;
  double leaves = 1.0;
  for (size_t k = d; k < _estimates.size(); ++k)
  {
    leaves *= _estimates[k].estimate;
    est += leaves;
  }
  ADM_DEBUG(estimator_v) << "On level d=" << d << " cut size=" << est
                         << std::endl
                         << std::endl;
  return est;
}

/******************************************************************************
 * @brief @todo.
 *
 * @param[in] d @todo.
 ******************************************************************************/
void Estimator::add_cut(const size_t d)
{
  #pragma omp critical
  {
    size_t& s = _estimates.at(d).n_samples;
    double& e = _estimates.at(d).estimate;
    e -= 1.0 / s;
    if (d < 4)
    {
      ADM_DEBUG(estimator_v)
          << "Removing Branch: depth=" << d << " new avg=" << e << std::endl;
    }
  }
}

/******************************************************************************
 * @brief @todo.
 *
 * @param[in] d @todo.
 * @param[in] v @todo.
 ******************************************************************************/
void Estimator::add_sample(const size_t d, const double& v)
{
  if (v < 1.0)
  {
    return;
  }
  #pragma omp critical
  {
    if (_estimates.size() <= d)
    {
      _estimates.resize(d + 1);
    }
    size_t& s = _estimates.at(d).n_samples;
    double& e = _estimates.at(d).estimate;
    e = (e * s + v);
    e /= ++s;
    if (d < 4)
    {
      ADM_DEBUG(estimator_v) << "Adding Estimate: depth=" << d << " value=" << v
                             << " new avg=" << e << std::endl;
    }
  }
}

}  // end namespace admission

// ################################## EOF ################################### //
