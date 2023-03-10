// ################################ INCLUDES ################################ //

#include "optimizers/branch_and_bound.hpp"

#include "graph/DAG.hpp"
#include "graph/tikz.hpp"
#include "lower_bounds/lower_bound.hpp"
#include "operations/find_eliminations.hpp"
#include "optimizers/optimizer_stats.hpp"
#include "util/openmp.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>

#include <algorithm>
#include <iostream>
#include <stddef.h>
#include <stdexcept>
#include <string>

// ############################ SOURCE CONTENTS ############################# //

namespace admission
{

// -------------------- BranchAndBound - Main Interface --------------------- //

/******************************************************************************
 * @brief Our own solve function, calling itself recursively.
 *
 * @param[in] g FaceDag& The input graph.
 * @param[in] cost_until_now flop_t Reference for comparison of global optima.
 * @param[in] parallel_depth flop_t Know the depth of recursion to switch
 *                           openMP task parallelisation.
 * @param[in] source VertexDesc Current Vertex in the MetaDAG.
 ******************************************************************************/
OpSequence BranchAndBound::branch_and_bound_solve(
    FaceDAG& g, const OpSequence solution_until_now,
    const flop_t parallel_depth, const VertexDesc source) const
{
  /* After _interval write a single-line output
   * providing information about solution progress. */
  static double time = get_wtime();
  if (get_wtime() - time > _interval)
  {
    #pragma omp critical
    {
      std::cout << (_output_mode ? "Est. Search Space: " : " ");
      double est = this->_est.calc_est(0);
      std::cout << est;
      std::cout << (_output_mode ? " Search Space frac: " : " ");
      std::cout << static_cast<double>(_stats.data[Branch]) / est;
      std::cout << (_output_mode ? " Curr. Opt.: " : " ") << _glob_opt_s.cost()
                << " ";
      if (_output_mode == true)
      {
        this->_stats.write_human_readable(std::cout);
      }
      else
      {
        this->_stats.write_log(std::cout);
      }
      std::cout << std::endl;
      time = get_wtime();
    }
  }

  /* If this DAG is already solved i.e.
   * no eliminations and accumulations can
   * be applied: return. */
  if (check_if_solved(g) == true)
  {
    this->_stats.add(Leaf);
    return OpSequence::make_empty();
  }

  /* Use two different greedy heuristics to produce
   * a preliminary result on this DAG. If it results
   * in a new global optimum, update it. This ensures "decent"
   * global solutions early in the solution process, even
   * if only few leaves were reached by the branch and bound. */
  FaceDAG* hp = new FaceDAG(g);
  auto branch_elims = _greedy_optimizer.greedy_solve(*hp, false);
  *hp = g;
  auto min_fill_in_elims = _min_fill_in_optimizer.min_fill_in_solve(*hp, false);
  *hp = g;
  auto tan_elims = _tangent_optimizer.tangent_solve(*hp);
  *hp = g;
  auto adj_elims = _adjoint_optimizer.adjoint_solve(*hp);
  *hp = g;

  branch_elims = (min_fill_in_elims.cost() < branch_elims.cost()) ?
                     min_fill_in_elims :
                     branch_elims;
  branch_elims = (tan_elims.cost() < branch_elims.cost()) ? tan_elims :
                                                            branch_elims;
  branch_elims = (adj_elims.cost() < branch_elims.cost()) ? adj_elims :
                                                            branch_elims;
  delete hp;
  #pragma omp critical
  {
    update_global_opt(branch_elims + solution_until_now);
  }

  /* Struct for counting the number of elims we will
   * execute. Passed as a callable object to traverse_elims.
   * To get an estimate of the search space. */
  struct count_elim
  {
    size_t _c = 0;

    void operator()(const FaceDAG&, const OpSequence&)
    {
      ++_c;
    }
  } ctr;

  traverse_elims(g, ctr, ctr);
  this->_est.add_sample(parallel_depth, ctr._c);

  /* Lambda for simplifying the branching, performing an operation
   * checking the bound and, in case, starting the subtree search.
   */
  auto branch_if = [&](const FaceDAG& g, OpSequence subbranch_elims)
  {
    this->_stats.add(Perf);

    FaceDAG* hp = new FaceDAG(g);
    subbranch_elims.apply(*hp);

    /* If _diagnostics are activated add a vertex to the meta DAG. */
    VertexDesc mv = 0;
    meta_dag_info_t* info = nullptr;
    if (_diagnostics)
    {
      #pragma omp critical
      {
        mv = add_vertex(_meta_dag);
        add_edge(source, mv, _meta_dag);
        info = &boost::get(boost::vertex_info, _meta_dag, mv);
      }
      admission::write_tikz(std::to_string(mv) + ".tex", *hp);
    }

    /* Check, whether the result should be searched recursively. */
    flop_t lb = (*(this->_lbound))(*hp);
    if ((lb + subbranch_elims.cost() + solution_until_now.cost() <
         _glob_opt_s.cost()) &&
        (lb + subbranch_elims.cost() < branch_elims.cost()))
    {
      #pragma omp task default(shared) \
                       firstprivate(subbranch_elims, parallel_depth) \
                       firstprivate(solution_until_now, hp, source, mv, info) \
                       final(parallel_depth >= _parallel_depth)
      {
        this->_stats.add(Branch);
        subbranch_elims += branch_and_bound_solve(
            *hp, solution_until_now + subbranch_elims, parallel_depth + 1, mv);

        /* Check if the optimal solution on h is better than the current
         * optimum. */
        #pragma omp critical
        {
          if (subbranch_elims.cost() < branch_elims.cost())
          {
            branch_elims = subbranch_elims;
            /* Check if this solution is better than the global optimum. */
            bool updated = update_global_opt(branch_elims + solution_until_now);
            if (updated && _diagnostics)
            {
              info->was_opt = true;
              boost::set_property(_meta_dag, boost::graph_opt, mv);
            }
          }
        }  // end omp critical
        delete hp;
      }  // end omp task
    }
    else
    {
      this->_est.add_cut(parallel_depth);
      this->_stats.add(Bound);
      if (_diagnostics)
      {
        info->cut = true;
      }
      delete hp;
    }
  };

  traverse_elims(g, branch_if, branch_if);
  #pragma omp taskwait

  return branch_elims;
}

/******************************************************************************
 * @brief Override the virtual solve function.
 *
 * @param[in] g FaceDAG& The input graph.
 * @returns OpSequence The optimal sequence.
 ******************************************************************************/
OpSequence BranchAndBound::solve(FaceDAG& g) const
{
  if (_lbound == nullptr)
  {
    throw std::runtime_error("Incomplete Solver");
  }
  this->_est.init(g);
  OpSequence res = OpSequence::make_max();

  #pragma omp parallel default(shared)
  {
    #pragma omp single
    {
      if (_diagnostics)
      {
        std::ofstream writer("0.tex");
        admission::write_tikz(writer, g);
        writer.close();
      }
      res = branch_and_bound_solve(
          g, OpSequence::make_empty(), 0, add_vertex(_meta_dag));
    }
  }

  return res;
}

// --------------- BranchAndBound - Internal solution helpers --------------- //

/******************************************************************************
 * @brief Traverses all possible preaccumulations and eliminations (operations)
 *        on a face DAG. For each operation, it performs some task specified
 *        by the callables passed to it.
 *
 * @tparam V_ACTION Callable vertex action.
 * @tparam E_ACTION Callable edge action.
 * @param[in] g The face DAG to traverse.
 * @param[in] vertex_action Callable to be executed on each
 *                          accumulatable vertex.
 * @param[in] edge_action Callable to be executed on each eliminatable edge.
 ******************************************************************************/
template<typename V_ACTION, typename E_ACTION>
void BranchAndBound::traverse_elims(
    const FaceDAG& g, V_ACTION& vertex_action, E_ACTION& edge_action) const
{
  auto Fprime_exists = boost::get(boost::vertex_acc_stat, g);
  auto Fbardot_exists = boost::get(boost::vertex_has_model, g);
  auto index = boost::get(boost::edge_index, g);
  const auto& [i_old, j_old, k_old] = boost::get_property(
      g, boost::graph_previous_op);

  BOOST_FOREACH(auto v, vertices(g))
  {
    auto&& [has_candidate, w] = has_merge_candidate(v, g);
    if (has_candidate == true)
    {
      vertex_action(g, cheapest_preacc(v, g));
    }
  }

  BOOST_FOREACH(auto ijk, edges(g))
  {
    VertexDesc ij = boost::source(ijk, g);
    VertexDesc jk = boost::target(ijk, g);

    /* Skip minimal and maximal edges as well as edges with
     * an index smaller than the last eliminated edge if
     */
    if (!in_degree(ij, g) || !out_degree(jk, g) ||
        (index(ijk) < j_old && index(ijk) != k_old && index(ijk) != i_old))
    {
      continue;
    }

    OpSequence ij_acc_s = cheapest_preacc(ij, g);
    OpSequence jk_acc_s = cheapest_preacc(jk, g);

    OpSequence adj_s = adj_prop(ijk, g);
    OpSequence tan_s = tan_prop(ijk, g);
    OpSequence mul_s = mul(ijk, g);

    flop_t ppm_c = ij_acc_s.cost() + jk_acc_s.cost() + mul_s.cost();
    flop_t pt_c = ij_acc_s.cost() + tan_s.cost();
    flop_t pa_c = jk_acc_s.cost() + adj_s.cost();

    // Case where both jacobians are accumulated.
    // We can
    //  - multiply
    //  - propagate v through w if w has a tangent model
    //  - propagate w through v if v has an adjoint model
    if (Fprime_exists[ij] == true && Fprime_exists[jk] == true)
    {
      if (Fbardot_exists[ij] && Fbardot_exists[jk])
      {
        if (adj_s.cost() <= tan_s.cost() && adj_s.cost() <= mul_s.cost())
        {
          edge_action(g, adj_s);
        }
        else if (tan_s.cost() <= mul_s.cost())
        {
          edge_action(g, tan_s);
        }
        else
        {
          edge_action(g, mul_s);
        }
      }
      else if (Fbardot_exists[ij])
      {
        if (adj_s.cost() < mul_s.cost())
        {
          edge_action(g, adj_s);
        }
        else
        {
          edge_action(g, mul_s);
        }
      }
      else if (Fbardot_exists[jk])
      {
        if (tan_s.cost() < mul_s.cost())
        {
          edge_action(g, tan_s);
        }
        else
        {
          edge_action(g, mul_s);
        }
      }
      else
      {
        edge_action(g, mul_s);
      }
    }
    else if (Fprime_exists[ij] == true && Fprime_exists[jk] == false)
    {
      if ((mul_s.cost() <= adj_s.cost() || !Fbardot_exists[ij]) &&
          (in_degree(jk, g) > 1 ||
           (jk_acc_s.cost() + mul_s.cost() < tan_s.cost())))
      {
        // Check the ACC and MUL case
        edge_action(g, jk_acc_s + mul_s);
      }

      if ((Fbardot_exists[ij] && adj_s.cost() < mul_s.cost()) &&
          (in_degree(jk, g) > 1 ||
           (jk_acc_s.cost() + adj_s.cost() < tan_s.cost())))
      {
        edge_action(g, jk_acc_s + adj_s);
      }

      if (in_degree(jk, g) > 1 ||
          ((jk_acc_s.cost() + adj_s.cost() >= tan_s.cost() ||
            !Fbardot_exists[ij]) &&
           jk_acc_s.cost() + mul_s.cost() >= tan_s.cost()))
      {
        // Check the TAN Propagation cost
        edge_action(g, tan_s);
      }
    }
    else if (Fprime_exists[ij] == false && Fprime_exists[jk] == true)
    {
      if ((mul_s.cost() < tan_s.cost() || !Fbardot_exists[jk]) &&
          (out_degree(ij, g) > 1 ||
           (ij_acc_s.cost() + mul_s.cost() < adj_s.cost())))
      {
        // Check the ACC and MUL case
        edge_action(g, ij_acc_s + mul_s);
      }
      if ((mul_s.cost() >= tan_s.cost() && Fbardot_exists[jk]) &&
          (out_degree(ij, g) > 1 ||
           (ij_acc_s.cost() + tan_s.cost() < adj_s.cost())))
      {
        // Check the ACC and MUL case
        edge_action(g, ij_acc_s + tan_s);
      }

      if (out_degree(ij, g) > 1 ||
          ((ij_acc_s.cost() + tan_s.cost() >= adj_s.cost() ||
            !Fbardot_exists[jk]) &&
           ij_acc_s.cost() + mul_s.cost() >= adj_s.cost()))
      {
        // Check the ADJ Propagation cost
        edge_action(g, adj_s);
      }
    }
    else  // both are unaccumulated
    {
      if (!Fbardot_exists[ij])
      {
        throw std::runtime_error{"A vertex has no model and no jacobian!"};
      }
      if (!Fbardot_exists[jk])
      {
        throw std::runtime_error{"A vertex has no model and no jacobian!"};
      }

      if (in_degree(jk, g) == 1 && out_degree(ij, g) == 1)
      {
        if (ppm_c == std::min({ppm_c, pt_c, pa_c}))
        {
          edge_action(g, ij_acc_s + jk_acc_s + mul_s);
        }
        else if (pt_c <= pa_c)
        {
          edge_action(g, ij_acc_s + tan_s);
        }
        else
        {
          edge_action(g, jk_acc_s + adj_s);
        }
      }
      else if (out_degree(ij, g) > 1 && in_degree(jk, g) == 1)
      {
        edge_action(g, jk_acc_s + adj_s);

        if (ppm_c <= pt_c)
        {
          edge_action(g, ij_acc_s + jk_acc_s + mul_s);
        }
        else
        {
          edge_action(g, ij_acc_s + tan_s);
        }
      }
      else if (out_degree(ij, g) == 1 && in_degree(jk, g) > 1)
      {
        edge_action(g, ij_acc_s + tan_s);

        if (ppm_c <= pa_c)
        {
          edge_action(g, ij_acc_s + jk_acc_s + mul_s);
        }
        else
        {
          edge_action(g, jk_acc_s + adj_s);
        }
      }
      else
      {
        // Accmumulate both and matmul
        edge_action(g, ij_acc_s + jk_acc_s + mul_s);
        // Accumulate v and propagate tangent
        edge_action(g, ij_acc_s + tan_s);
        // Accumulate w and propagate adjoint
        edge_action(g, jk_acc_s + adj_s);
      }
    }
  }  // end FOR_EACH over all edges
}

}  // end namespace admission

// ################################## EOF ################################### //
