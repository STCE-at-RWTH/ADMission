#ifndef ESTIMATOR_HPP
#define ESTIMATOR_HPP

// ******************************** Includes ******************************** //

#include "admission_config.hpp"
#include "graph/DAG.hpp"

#include <boost/graph/adjacency_list.hpp>

#include <iostream>
#include <memory>
#include <stddef.h>
#include <vector>

// **************************** Header contents ***************************** //

namespace admission
{

/**
 * \addtogroup Optimizers
 * \ @{
 */

/**\brief Struct to estimate the length size of a branch and bound search space */
struct Estimator
{
  struct Data
  {
    size_t n_samples = 0;
    // size_t n_cut = 0;
    double estimate = 1;
  };

  Estimator() = default;

  void init(const FaceDAG& g)
  {
    _estimates.reserve(2 * num_edges(g) + num_vertices(g));
    _estimates.resize(0);
  }

  double calc_est(const size_t d)
  {
    // We first calculate an estimate of the entire search tree without
    // cutting branches.
    double est = 1.0;
    double leaves = 1.0;
    for (size_t k = d; k < _estimates.size(); ++k)
    {
      leaves *= _estimates[k].estimate;
      est += leaves;
    }
    // ADM_DEBUG(estimator_v) << "On level d=" << d << " uncut size=" << est << std::endl;

    // // An then subtract the estimated size of all cut brunches.
    // std::vector<double> _cuts(_estimates.size(), 1.0);
    // double est_cut = 1.0;
    // double est_leaves = 1.0;
    // double pos_cut = 1.0;
    // for (size_t n=d; n<_estimates.size(); ++n) {
    //   for (size_t k=d; k<n+1; ++k) {
    //     pos_cut *=_estimates[k].estimate;
    //   }
    //   for (size_t k=n+1; k<_estimates.size(); ++k) {
    //     est_leaves *= _estimates[k].estimate;
    //     est_cut += est_leaves;
    //   }
    //   ADM_DEBUG(estimator_v) << "On level n=" << n
    //     << "average children=" << _estimates[n].estimate
    //     << " possible cuts= " << pos_cut << " cuts=" << _estimates[n].n_cut
    //     << " with total size=" << est_cut * _estimates[n].n_cut << std::endl;

    //   est -= est_cut * _estimates[n].n_cut;
    //   est_cut = 1.0;
    //   pos_cut = 1.0;
    //   est_leaves = 1.0;
    // }
    ADM_DEBUG(estimator_v) << "On level d=" << d << " cut size=" << est
                           << std::endl
                           << std::endl;
    return est;
  }

  void add_cut(const size_t d)
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

  void add_sample(const size_t d, const double& v)
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
      e = (e * s + v) / (s + 1);
      if (d < 4)
      {
        ADM_DEBUG(estimator_v)
            << "Adding Estimate: depth=" << d << " value=" << v
            << " new avg=" << e << std::endl;
      }
    }
  }

  std::vector<Data> _estimates;
};

/**
 * @}
 */

}  // end namespace admission

#endif  // ESTIMATOR_HPP
