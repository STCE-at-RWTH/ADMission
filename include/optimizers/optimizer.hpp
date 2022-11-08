#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

// ******************************** Includes ******************************** //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "operations/op_sequence.hpp"
#include "optimizers/optimizer_stats.hpp"
#include "factory.hpp"

#include <filesystem>
#include <iostream>

// ************************** Forward declarations ************************** //

namespace admission
{
class LowerBound;
}

// **************************** Header contents ***************************** //

namespace admission
{

/**
 * \defgroup Optimizers Optimizers. Algorthimgs for solving Jacobian Accumulation on a FaceDAG.
 * \addtogroup Optimizers
 * \ @{
 */

/** \brief Abstract optimizer class.
 */
class Optimizer
{
 public:
  /// Trivial typedef: This is the transitive base of itself.
  typedef Optimizer AbstractBase;

 public:
  ///\name Constructor and Destructor.
  ///@{
  Optimizer() {}

  virtual ~Optimizer() {}

  ///@}

  ///\name Optimizer settings.
  ///@{

  /// Turn on tracking of the search space and writing intermediate solutions.
  virtual void set_diagnostics(bool on)
  {
    _diagnostics = on;
  }

  virtual void set_output_interval(double t)
  {
    _interval = t;
  }

  virtual void set_output_mode(bool hr)
  {
    _output_mode = hr;
  }

  /// Reset the internal elimination and branch/cut counters.
  virtual void reset()
  {
    _stats.reset();
  }

  ///@}

  ///\name Virtual functions to query certain members.
  ///@{

  /// Returns true if the optimizer uses a lower bound. Implemented by derived classes.
  virtual bool has_lower_bound() = 0;

  /// Returns true if the optimizer can be parallelised. Implemented by derived classes.
  virtual bool is_parallel() = 0;

  /// Can be used to set the depth of openMP task directive on parallel optimizers. Implemented bt derived classes.
  virtual void set_parallel_depth(const plength_t d) = 0;

  /// Set the lower bound by reference. Implemented by derived classes.
  virtual void set_lower_bound(admission::LowerBound&) = 0;

  /// Set the lower bound by pointer.   Implemented by derived classes.
  virtual void set_lower_bound(admission::LowerBound*) = 0;

  /// Returns a pointer to the lower bounds. Implemented by derived classes.
  virtual const LowerBound* get_lower_bound() = 0;

  ///@}

  ///\name IO.
  ///@{
  /**\brief Prints the optimizer's stats
   *
   * @param os std::ostream& the stream to write to.
   */
  void write(std::ostream& os = std::cout)
  {
    _stats.write(os);
  }

  /**\brief Prints the opsimiser's stats in a less verbose form.
   *
   * @param os std::ostream& the stream to write to.
   */
  void write_log(std::ostream& os = std::cout)
  {
    _stats.write_log(os);
  }

  /**\brief Prints the meta DAG that is build when _diagnostics is turned on.
   *
   * @param o reference to an ostream to print to.
   */
  virtual void print_meta_DAG(std::ostream& o = std::cout) const;

  /**\brief Prints the meta DAG that is build when _diagnostics is turned on.
   *
   * @param p fs::path of the file to write to.
   */
  virtual void print_meta_DAG(fs::path p) const;
  ///@}

  ///\name Main interface.
  ///@{
  /**\brief Solve matrix free vector face elimination on
   *        a face DAG. Implemented by derived classes.
   *
   * @param g Reference to the FaceDAG to solve.
   * @returns OpSequence the solution.
   */
  virtual OpSequence solve(FaceDAG&) const = 0;
  ///@}

 protected:
  ///\name Internal solution helpers.
  ///@{
  /** \brief  Used to determine whether a DAG is solved.
   *
   * @param g FaceDAG& the face DAG to check.
   * @returns bool true if solved.
   */
  bool check_if_solved(const FaceDAG& g) const;

 protected:
  /// Stats to track the branching and cutting behaviour of the Optimizer.
  mutable OptimizerStats _stats;

  /// MetaDAG to track the search space exploded. Left empty if diagnostics are turned off.
  mutable MetaDAG _meta_dag;

  bool _output_mode = true;
  double _interval = 1.0;

  /// Diagnostics switch.
  bool _diagnostics = false;
};

/// Make an optimizer factory
using OptimizerFactory = admission::Factory<Optimizer>;

/**
 * \ @}
 */

}  // end namespace admission

#endif  // OPTIMIZER_HPP
