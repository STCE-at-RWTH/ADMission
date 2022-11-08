// ******************************** Includes ******************************** //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "graph/face_dag.hpp"
#include "graph/read_graph.hpp"
#include "operations/accumulate.hpp"
#include "operations/op_sequence.hpp"
#include "elimination_algorithm.hpp"
#include "properties.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <omp.h>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

// **************************** Source contents ***************************** //

/**
 * Verifies that two face DAGs
 * \f$a = (\tilde V_a, \tilde E_a)\f$
 * \f$b = (\tilde V_b, \tilde E_b)\f$
 *    Compute an isomorphism using boost::isomorphism.
 *    Check for identical elemental Jacobians:
 *      For each intermediate vertex \f$(x,y) \in \tilde V_a\f$.
 *        Find the corresponding vertex \f$(x,y) \in \tilde V_b\f$.
 *        Assert that the in- and out-degrees of both vertices are 1.
 *        Assert that \f$n_a((x, (x,y))) == n_b((x, (x,y)))\f$ and \f$n_a(((x,y), y)) == n_b(((x,y), y))\f$.
 * @param[in] a First Graph
 * @param[in] b Second Graph
 * @returns A pair of bool (true, if the graphs are equal) and string (a description
 * of the first difference between a and b that was found).
 */
std::pair<bool, std::string> verify_equal_DAGs(
    admission::FaceDAG& a, admission::FaceDAG& b)
{
  auto ind = boost::get(boost::edge_index, a);

  /* Removes all isolated vertices from a face DAG, because eliminate_edge
   * will not delete isolated vertices for the sake of speed.
   */
  auto remove_isolated = [&](admission::FaceDAG& g)
  {
    auto remove_one = [&]()
    {
      BOOST_REVERSE_FOREACH(auto v, vertices(g))
      {
        if ((in_degree(v, g) == 0) && (out_degree(v, g) == 0))
        {
          remove_vertex(v, g);
          return false;
        }
      }
      return true;
    };
    while (remove_one() == false)
    {}
  };

  remove_isolated(a);
  remove_isolated(b);

  BOOST_FOREACH(auto v, vertices(a))
  {
    if ((out_degree(v, a) == 1) && (in_degree(v, a) == 1))
    {
      auto vi = admission::index_pair(v, a);
      bool found_match = false;

      BOOST_FOREACH(auto w, vertices(b))
      {
        if ((out_degree(w, b) == 1) && (in_degree(w, b) == 1) &&
            (admission::index_pair(w, b) == vi))
        {
          found_match = true;
        }
      }
      if (!found_match)
      {
        return std::make_pair(
            false, "No match found for vertex (" +
                       std::to_string(ind[*(in_edges(v, a).first)]) + ", " +
                       std::to_string(ind[*(out_edges(v, a).first)]) +
                       ") of graph a!");
      }
    }
  }
  return std::make_pair(true, "OK");
}

/**Stores properties of the verification algorithm.
 * See register_property calls in the constructor
 * for details.
 */
class VerifyProperties : public admission::Properties
{
 public:
  typedef VerifyProperties P;

  fs::path graph_path = "";
  fs::path seq_path = "";

  bool preaccumulate_all = false;

  VerifyProperties()
  {

    register_property(
        preaccumulate_all, "preaccumulate_all",
        "Will treat the input DAG as if no tan/adj models were available and "
        "all elemental Jacobians were preaccumulated.");
    register_property(graph_path, "graph_path", "Path to the DAG.");

    register_property(seq_path, "seq_path", "Path to the sequence.");
  }
};

/**User interface for checking if an elimination sequence s
 * applied to \tilde G, the dual of a DAG G results in
 * a valid tripartite face DAG.
 * The algorithm compares the result of the application
 * of s on \tilde G to a trivial way of accumulation
 * as defined in accumulate_trivial().
 */
int main(int argc, char* argv[])
{
  VerifyProperties p;

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

  // Create and read a DAG.
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
  // Build a FaceDAG out of the parsed DAG.
  auto g_f = admission::make_face_dag(g);

  // Request a sequence path and fill a sequence.
  auto s = admission::OpSequence::make_empty();
  read_sequence(std::ifstream(p.seq_path), s);

  if (p.preaccumulate_all == true)
  {
    std::cout << "Preaccumulating all elemental Jacobians..." << std::endl;
    admission::preaccumulate_all(*g_f);
  }

  // Use an accumulator to apply the seq, accum. the trivial way and compare.
  auto time = omp_get_wtime();
  std::cout << "Accumulating according to OpSequence... " << std::endl;
  s.apply(*g_f);
  std::cout << "...done." << std::endl;
  std::cout << "Accumulating trivially... " << std::endl;
  auto acc_g_f = admission::accumulate_trivial(g);
  std::cout << "...done." << std::endl;
  auto [ok, msg] = verify_equal_DAGs(*g_f, *acc_g_f);
  time = omp_get_wtime() - time;
  std::cout << "Elapsed time " << time << "s" << std::endl;
  std::cout << "Result: ";
  if (ok)
  {
    std::cout << "ok" << std::endl;
  }
  else
  {
    std::cout << "error (" << msg << ")" << std::endl;
  }

  return 0;
}
