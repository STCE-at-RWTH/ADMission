#ifndef ADM_INC_OPTIMIZERS_OPTIMIZER_HPP_
#define ADM_INC_OPTIMIZERS_OPTIMIZER_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "misc/doxygen.hpp"
#include "operations/op_sequence.hpp"
#include "optimizers/optimizer_stats.hpp"
#include "util/factory.hpp"

#include <filesystem>
#include <iostream>

// ########################## FORWARD DECLARATIONS ########################## //

namespace admission
{
class LowerBound;
}

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(Optimizers)

/******************************************************************************
 * @brief Abstract optimizer class.
 ******************************************************************************/
class Optimizer
{
 public:
  //! Trivial typedef: This is the transitive base of itself.
  typedef Optimizer AbstractBase;

 public:
  //! Trivial default constructor.
  Optimizer() = default;

  //! Trivial destructor.
  virtual ~Optimizer() = default;

  // -------------------------- Optimizer settings -------------------------- //
  DOXYGEN_GROUP_BEGIN(Optimizer settings, )

  //! Turn on tracking of the search space and writing intermediate solutions.
  virtual void set_diagnostics(bool on);

  //! Set the interval of writing single-line output.
  virtual void set_output_interval(double t);

  //! Set the mode of the output to human readable or shorter.
  virtual void set_output_mode(bool hr);

  //! Reset the internal elimination and branch/cut counters.
  virtual void reset();

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //

  // -------------- Virtual functions to query certain members -------------- //
  DOXYGEN_GROUP_BEGIN(Virtual functions to query certain members, )

  //! Returns true if the optimizer uses a lower bound. Implemented by derived
  //! classes.
  virtual bool has_lower_bound() = 0;

  //! Returns true if the optimizer can be parallelised. Implemented by derived
  //! classes.
  virtual bool is_parallel() = 0;

  //! Can be used to set the depth of openMP task directive on parallel
  //! optimizers. Implemented bt derived classes.
  virtual void set_parallel_depth(const plength_t d) = 0;

  //! Set the lower bound by reference. Implemented by derived classes.
  virtual void set_lower_bound(admission::LowerBound&) = 0;

  //! Set the lower bound by pointer.   Implemented by derived classes.
  virtual void set_lower_bound(admission::LowerBound*) = 0;

  //! Returns a pointer to the lower bounds. Implemented by derived classes.
  virtual const LowerBound* get_lower_bound() = 0;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //

  // --------------------------------- I/O ---------------------------------- //
  DOXYGEN_GROUP_BEGIN(I / O, )

  //! Prints the optimizer's stats.
  void write(std::ostream& os = std::cout);

  //! Prints the opsimiser's stats in a less verbose form.
  void write_log(std::ostream& os = std::cout);

  //! Prints the meta DAG that is build when _diagnostics is turned on.
  virtual void print_meta_DAG(std::ostream& o = std::cout) const;

  //! Prints the meta DAG that is build when _diagnostics is turned on.
  virtual void print_meta_DAG(fs::path p) const;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //

  // ---------------------------- Main interface ---------------------------- //
  DOXYGEN_GROUP_BEGIN(Main interface, )

  //! Solve matrix free vector face elimination on
  //! a face DAG. Implemented by derived classes.
  virtual OpSequence solve(FaceDAG&) const = 0;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //

 protected:
  // ---------------------- Internal solution helpers ----------------------- //
  DOXYGEN_GROUP_BEGIN(Internal solution helpers, )

  //! Used to determine whether a DAG is solved.
  bool check_if_solved(const FaceDAG& g) const;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //

 protected:
  //! Stats to track the branching and cutting behaviour of the Optimizer.
  mutable OptimizerStats _stats;

  //! MetaDAG to track the search space exploded. Left empty if diagnostics are
  //! turned off.
  mutable MetaDAG _meta_dag;

  //! @todo
  bool _output_mode = true;

  //! @todo
  double _interval = 1.0;

  //! Diagnostics switch.
  bool _diagnostics = false;
};

//! Make an optimizer factory.
using OptimizerFactory = admission::Factory<Optimizer>;

DOXYGEN_MODULE_END(Optimizers)

}  // end namespace admission

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "optimizers/impl/optimizer.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_OPTIMIZERS_OPTIMIZER_HPP_
