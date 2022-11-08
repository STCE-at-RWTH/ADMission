// ******************************** Includes ******************************** //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "graph/face_dag.hpp"
#include "graph/join.hpp"
#include "graph/read_graph.hpp"
#include "graph/tikz.hpp"
#include "lower_bounds/lower_bound.hpp"
#include "lower_bounds/simple_min_acc_cost_bound.hpp"
#include "operations/op_sequence.hpp"
#include "optimizers/all_optimizers.hpp"
#include "optimizers/optimizer.hpp"
#include "elimination_algorithm.hpp"
#include "factory.hpp"
#include "global_modes.hpp"
#include "properties.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <omp.h>
#include <stddef.h>
#include <stdexcept>
#include <string>

// **************************** Source contents ***************************** //

/**Stores all relevant information for reading a DAG from memory
 * and solving JA on it. Details of the single parameters' purpose
 * can be found in their respective registration.
 */
class SolveProperties : public admission::Properties
{
 public:
  typedef SolveProperties P;

  fs::path graph_path = "";
  std::string optimizer_name = "";
  size_t n_threads = 1;
  size_t n_levels = 1;
  bool diagnostics = false;
  bool join_vertices = true;
  bool preaccumulate_all = false;
  double solution_output_interval = 1.0;
  bool human_readable = true;

  SolveProperties()
  {
    register_property(graph_path, "graph_path", "Path to the DAG.");
    register_property(
        optimizer_name, "optimizer_name",
        "Identifier of the optimizer to solve FE.");
    register_property(
        n_threads, "n_threads",
        "Max number of threads to use in a parallel optimizer and parallel "
        "sparsity pattern computation.");
    register_property(
        n_levels, "thread_spawn_depth",
        "Max depth of the search tree in which searching of branches is "
        "delegated to OpenMP tasks.");
    register_property(
        diagnostics, "print_diagnostics",
        "Enable diagnostic output of branch and bound.");
    register_property(
        join_vertices, "join_vertices",
        "Join vertices with identical predecessor and successor sets before "
        "solving.");
    register_property(
        preaccumulate_all, "preaccumulate_all",
        "Will preaccumulate all Jacobians and solve JA for the preaccumulated "
        "DAG without tan/adj models.");
    register_property(
        solution_output_interval, "solution_output_interval",
        "Select the frequency of solution updates to write. Defalt is 1s.");
    register_property(
        human_readable, "human_readable_output",
        "Print solution update as a single line or in log style.");
  }
};

/**Interface for solving
 * matrix free   vector face   elimination
 * m      f      v      f      e
 * on a DAG.
 */
int main(int argc, char* argv[])
{
  SolveProperties p;

  if (argc != 2)
  {
    p.print(std::cout);
    return -1;
  }

  try
  {
    p.parse_config(std::ifstream(argv[1]));
  }
  catch (const admission::BadConfigFileError& bcfe)
  {
    std::cout << bcfe.what() << std::endl;
    return -1;
  }
  catch (const admission::KeyNotRegisteredError& knre)
  {
    std::cout << knre.what() << std::endl;
    return -1;
  }

  // Create a DAG and parse it from the provided path.
  admission::DAG g;
  try
  {
    admission::read_graph(p.graph_path, g);
  }
  catch (std::runtime_error& e)
  {
    std::cout << e.what() << std::endl;
    return -1;
  }

  // Join vertices because why did we invent vector DAGs anyway?
  if (p.join_vertices == true)
  {
    admission::join_vertices(g);
  }

  std::cout << "Printing DAG to TikZ for you...\n";

  std::ofstream o("admission_input.tex");
  o << "\\documentclass{standalone}" << std::endl;
  o << "\\usepackage{graphicx}" << std::endl;
  o << "\\usepackage{tikz}" << std::endl;
  o << "\\usetikzlibrary{shapes,arrows,automata,decorations.pathreplacing,"
       "angles,quotes}"
    << std::endl;
  o << "\\begin{document}" << std::endl;
  admission::write_tikz(o, g);
  o << "\\end{document}" << std::endl;
  o.close();

  // Build a FaceDAG out of the parsed graph.
  auto g_f = admission::make_face_dag(g);

  // If global preaccumulation is desired, print its cost.
  admission::OpSequence s_mm = admission::OpSequence::make_empty();
  if (p.preaccumulate_all == true)
  {
    s_mm = admission::global_preaccumulation_ops(*g_f);
    admission::preaccumulate_all(*g_f, true);
    std::cout << "Local Preaccumulation cost: " << s_mm.cost() << "fma\n";
  }

  // Compute the cost of global modes.
  admission::flop_t c_t = admission::global_tangent_cost(*g_f);
  admission::flop_t c_a = admission::global_adjoint_cost(*g_f);

  // Create an optimizer.
  admission::Optimizer* op;
  static auto optimizer_factory = admission::OptimizerFactory::instance();

  try
  {
    op = optimizer_factory->construct(p.optimizer_name);
  }
  catch (admission::KeyNotRegisteredError& tnr)
  {
    std::cout << tnr.what() << std::endl;
    return -1;
  }

  // Create a lower bound if needed.
  if (op->has_lower_bound())
  {
    op->set_lower_bound(new admission::SimpleMinAccCostBound());
    op->set_output_interval(p.solution_output_interval);
    op->set_output_mode(p.human_readable);
  }

  // Set the optimizer settings.
  if (op->is_parallel())
  {
    op->set_parallel_depth(p.n_levels);
    omp_set_num_threads(p.n_threads);
  }
  op->set_diagnostics(p.diagnostics);

  // Solve and write the solution and timings to console.
  std::cout << "Planning AD Mission...\n";

  auto time = omp_get_wtime();
  auto seq = op->solve(*g_f);
  time = omp_get_wtime() - time;
  std::cout << std::endl
            << "Done after " << time << "s"
            << "\n\n"
            << "Op |Dir|Where|c(fma)"
            << "\n";
  seq.write(std::cout);
  std::cout << "\n";

  std::ofstream sof("admission.out");
  seq.write(sof);
  sof.close();

  if (dynamic_cast<admission::BranchAndBound*>(op) != nullptr)
  {
    op->write(std::cout);
  }

  if (p.preaccumulate_all == true)
  {
    std::cout << "Local Preaccumulation cost: " << s_mm.cost() << "fma\n";
  }
  std::cout << "Global tangent cost: " << c_t << "fma\n"
            << "Global adjoint cost: " << c_a << "fma\n"
            << "Optimized cost: " << seq.cost() << "fma\n"
            << "Operations: " << seq.size() << std::endl;

  if (p.diagnostics)
  {
    op->print_meta_DAG("metaDAG.tex");
  }

  if (op->has_lower_bound())
  {
    delete op->get_lower_bound();
  }

  delete op;
  return 0;
}
