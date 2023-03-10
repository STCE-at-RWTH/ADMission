// ################################ INCLUDES ################################ //

#include "misc/newton_steps.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <assert.h>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

// ############################ SOURCE CONTENTS ############################# //

namespace admission
{

/******************************************************************************
 * @brief From a single-newton-step-DAG creates a multi-newton-step-DAG.
 *
 * @param[in] n1 DAG representing a single newton step.
 * @param[in] n  Number of steps to generate.ß
 * @returns std::shared_ptr to the multi-step DAG.
 ******************************************************************************/
std::shared_ptr<DAG> make_newton_steps(DAG& n1, unsigned int n)
{
  /* Identify all nodes:
   *    x_0 : Minimal Vertex with 3 successors.
   *    p   : Minimal Vertex with 2 successors.
   *    x_1 : Maximal vertex with 3 predecessors.
   *    r_0 : Intermediate vertex with 2 predecessors.
   *    dr_0: Intermediate vertex with 2 predecessors.
   *
   *    x1
   *    |\\
   *    | \\
   *    | | \
   *    | r dr
   *    | |\/\
   *    | //\ \
   *    |//  \|
   *    x0    p
   */

  // We intitialize all named vertices to the gratest possible vertex index.
  constexpr VertexDesc magic_unassigned =
      std::numeric_limits<VertexDesc>::max();

  // Find the vertices by the number of pre- and successors.
  std::vector<VertexDesc> named_verts(5, magic_unassigned);
  VertexDesc& x0 = named_verts[0];
  VertexDesc& p = named_verts[1];
  VertexDesc& r = named_verts[2];
  VertexDesc& dr = named_verts[3];
  VertexDesc& x1 = named_verts[4];

  // Traverse over all vertices of the input DAG.
  BOOST_FOREACH(VertexDesc i, vertices(n1))
  {
    if (out_degree(i, n1) == 3 && in_degree(i, n1) == 0 &&
        x0 == magic_unassigned)
    {
      x0 = i;
    }
    else if (out_degree(i, n1) == 2 && in_degree(i, n1) == 0 && p == magic_unassigned)
    {
      p = i;
    }
    else if (
        out_degree(i, n1) == 0 && in_degree(i, n1) == 3 &&
        x1 == magic_unassigned)
    {
      x1 = i;
    }
    else if (in_degree(i, n1) == 2 && out_degree(i, n1) == 1 && r == magic_unassigned)
    {
      r = i;
    }
    else if (
        in_degree(i, n1) == 2 && out_degree(i, n1) == 1 &&
        dr == magic_unassigned)
    {
      dr = i;
    }
    else
    {
      throw std::runtime_error("Could not associate an input vertex!");
    }
  }

  // Verify, that all vertices are associated.
  for (VertexDesc& i : named_verts)
  {
    if (i == magic_unassigned)
    {
      throw std::runtime_error("Named vertex left unassigned!");
    }
  }

  // Create output Newton DAG with n steps.
  auto nnp = std::make_shared<DAG>();
  DAG& nn = *nnp;

  auto add_vertex_from = [&](VertexDesc from)
  {
    auto to = boost::add_vertex(nn);
    boost::put(
        boost::vertex_size, nn, to, boost::get(boost::vertex_size, n1, from));
    return to;
  };

  auto add_edge_from =
      [&](VertexDesc i, VertexDesc j, VertexDesc i1, VertexDesc j1)
  {
    auto [from, n1_succ] = boost::add_edge(i1, j1, n1);
    assert(n1_succ == false && "The from edge did not exist in n1!");
    auto [to, succ] = boost::add_edge(i, j, nn);
    assert(succ == true && "Tried to add an edge that already exists!");
    boost::put(
        boost::edge_acc_stat, nn, to,
        boost::get(boost::edge_acc_stat, n1, from));
    boost::put(
        boost::edge_has_model, nn, to,
        boost::get(boost::edge_has_model, n1, from));
    boost::put(
        boost::edge_c_tan, nn, to, boost::get(boost::edge_c_tan, n1, from));
    boost::put(
        boost::edge_c_adj, nn, to, boost::get(boost::edge_c_adj, n1, from));
    return to;
  };

  // Build the inputs of the first step.
  VertexDesc nn_x0 = add_vertex_from(x0);
  VertexDesc nn_p = add_vertex_from(p);

  // Build all subsequent steps.
  for (unsigned int k = 0; k < n; ++k)
  {
    VertexDesc nn_r = add_vertex_from(r);
    add_edge_from(nn_x0, nn_r, x0, r);
    add_edge_from(nn_p, nn_r, p, r);

    VertexDesc nn_dr = add_vertex_from(dr);
    add_edge_from(nn_x0, nn_dr, x0, dr);
    add_edge_from(nn_p, nn_dr, p, dr);

    VertexDesc nn_x1 = add_vertex_from(x1);
    add_edge_from(nn_r, nn_x1, r, x1);
    add_edge_from(nn_dr, nn_x1, dr, x1);
    add_edge_from(nn_x0, nn_x1, x0, x1);

    nn_x0 = nn_x1;
  }
  return nnp;
}

}  // end namespace admission

// ################################## EOF ################################### //
