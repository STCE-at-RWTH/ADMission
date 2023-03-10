// ################################ INCLUDES ################################ //

#include "operations/elimination_algorithms.hpp"

#include "admission_config.hpp"
#include "graph/neighbors.hpp"
#include "operations/find_eliminations.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/pending/property.hpp>

#include <iostream>
#include <utility>

// ############################ SOURCE CONTENTS ############################# //

namespace admission
{

namespace
{

/******************************************************************************
 * @brief Performs an absorption according to (Rule 2) (1.).
 *
 * @param[in] ij VertexDesc of \f$(i,*,j)\f$.
 * @param[in] jk VertexDesc of \f$(j,*,k)\f$.
 * @param[in] ik VertexDesc of the absorbing vertex \f$(i,*,k)\f$.
 * @param[inout] g Reference to the face DAG.
 ******************************************************************************/
ADM_ALWAYS_INLINE void absorb(
    const VertexDesc ij, const VertexDesc jk, const VertexDesc ik, FaceDAG& g)
{
  // Set c_tan(i,*,k) to zero.
  put(boost::vertex_c_tan, g, ik, 0);
  // Set c_adj(i,*,k) to zero.
  put(boost::vertex_c_adj, g, ik, 0);
  // F'(i,*,k) != \emptyset. Onlt relevant if F'(i,*,k) was
  // preaccumulated above.
  put(boost::vertex_acc_stat, g, ik, true);
  // \dot F(i,*,k) = \bar F(i,*,k) = \emptyset.
  put(boost::vertex_has_model, g, ik, false);

  ADM_DEBUG(eliminate_edge_detail_v) << "Absorbing result of " << jk << " and "
                                     << ij << " into " << ik << std::endl;
}

/******************************************************************************
 * @brief Generates a fill in (i,*,j,*,k) and connects it to the
 *        predecessors P(i,*,j) of (i,*,j) and the successors S(j,*,k)
 *        of (j,*,k) according to (Rule 2) (1.).
 *
 * @param[in] ij VertexDesc of (i,*,j).
 * @param[in] jk VertexDesc of (j,*,k).
 * @param[inout] g Reference to the face DAG.
 ******************************************************************************/
ADM_ALWAYS_INLINE void fill_in(
    const VertexDesc ij, const VertexDesc jk, FaceDAG& g)
{
  ADM_DEBUG(eliminate_edge_detail_v)
      << "Generating fill-in as result of " << ij << " -- " << jk << std::endl;

  // Add (i,*,j,*,k) to g.
  VertexDesc ik = add_vertex(g);

  // Add all required edges to ik such
  // that S(i,*,j,*,k) = S(j,*,k).
  BOOST_FOREACH(auto oe, out_edges(jk, g))
  {
    auto new_e = add_edge(ik, target(oe, g), g);
    put(boost::edge_index, g, new_e.first, get(boost::edge_index, g, oe));
    put(boost::edge_size, g, new_e.first, get(boost::edge_size, g, oe));
  }

  // Add all required edges to ik such
  // that P(i,*,j,*,k) = P(i,*,j).
  BOOST_FOREACH(auto ie, in_edges(ij, g))
  {
    auto new_e = add_edge(source(ie, g), ik, g);
    put(boost::edge_index, g, new_e.first, get(boost::edge_index, g, ie));
    put(boost::edge_size, g, new_e.first, get(boost::edge_size, g, ie));
  }

  // Set c_tan(i,*,j,*,k) to zero.
  put(boost::vertex_c_tan, g, ik, 0);
  // Set c_adj(i,*,j,*,k) to zero.
  put(boost::vertex_c_adj, g, ik, 0);
  // F'(i,*,j,*,k) != \emptyset.
  put(boost::vertex_acc_stat, g, ik, true);
  // \dot F(i,*,j,*,k) = \bar F(i,*,j,*,k) = \emptyset.
  put(boost::vertex_has_model, g, ik, false);
}

/******************************************************************************
 * @brief Merges two vertices, i.e. just removes the "from" vertex.
 *        Corresponds to (Rule 2) (4.) (merge).
 *
 * @param[in] from VertexDesc of the vertex to be merged.
 * @param[in] into VertexDesc of the vertex into which the other one is merged.
 * @param[inout] g Reference to the face DAG.
 ******************************************************************************/
ADM_ALWAYS_INLINE void merge_vertices(
    const VertexDesc from, const VertexDesc into, FaceDAG& g)
{
  ADM_DEBUG(eliminate_edge_detail_v)
      << "Merging vertex " << from << " into " << into << std::endl;

  // F'(i,*,j,*,k) != \emptyset.
  put(boost::vertex_has_model, g, into, false);
  // \dot F(i,*,j,*,k) = \bar F(i,*,j,*,k) = \emptyset.
  put(boost::vertex_acc_stat, g, into, true);

  // Isolate the vertex in both directions but not remove it,
  // saving us from copying elements of the vector storage
  // used by the FaceDAG class.
  clear_vertex(from, g);
}

/******************************************************************************
 * @brief (Rule 2) (4. and 5.) Removes a vertex (i,*,j) if it is bottom- or
 *        top-isolated. Tries to merge it with a parallel vertex otherwise.
 *
 * @param[in] ij VertexDesc of the vertex (i,*,j) to remove or merge.
 *           if a forced merge happens.
 * @param[inout] g Reference to the face DAG.
 ******************************************************************************/
ADM_ALWAYS_INLINE void remove_or_merge(const VertexDesc ij, FaceDAG& g)
{
  ADM_DEBUG(eliminate_edge_detail_v)
      << "Trying to remove or merge vertex " << ij << std::endl;

  // If S(i,*,j) = \emptyset or P(i,*j) = \emptyset.
  // This covers the conditions from (Rule 2) (4. and 5.).
  if (in_degree(ij, g) == 0 || out_degree(ij, g) == 0)
  {
    // Isolate the vertex in both directions but not remove it,
    // saving us from copying elements of the vector storage
    // used by the FaceDAG class.
    clear_vertex(ij, g);
    ADM_DEBUG(eliminate_edge_detail_v) << "Removed vertex " << ij << std::endl;
  }
  // ElseIf F'(i,*,j) = \emptyset. We do nothing.
  else if (get(boost::vertex_acc_stat, g, ij) == false)
  {
    ADM_DEBUG(eliminate_edge_detail_v)
        << "Did not remove or merge vertex " << ij << std::endl;
  }
  // ElseIf F'(i,*,j) != \emptyset. We try to merge.
  else
  {
    // Iterate over all successors (j,k) of (i,j)
    BOOST_FOREACH(auto oe, out_edges(ij, g))
    {
      VertexDesc jk = target(oe, g);

      // ... and over all predecessors (h,j) of all successors (j,k).
      BOOST_FOREACH(auto ie, in_edges(jk, g))
      {
        VertexDesc hj = source(ie, g);
        if (hj == ij)
        {
          continue;
        }

        // Check if succ. set and pred. set of j and v are identical
        // P(i,*,j) = P(*,j) && S(i,*,j) = S(*,j) && F'(*,j) != \emptyset
        if (same_predecessors(g, hj, ij) && same_successors(g, hj, ij) &&
            get(boost::vertex_acc_stat, g, hj))
        {
          merge_vertices(hj, ij, g);
          return;
        }
      }
    }
  }
  ADM_DEBUG(eliminate_edge_detail_v)
      << "Did not remove or merge vertex " << ij << std::endl;
}

}  // end anonymous namespace

/******************************************************************************
 * @brief (Rule 2)(1.) Checks if the the resulting vertex from eliminating
 *        (i,j,k) can be absorbed into another vertex (i,*,k).
 *
 * @param[in] ij VertexDesc of the source of the edge (i,j,k).
 * @param[in] jk VertexDesc of the target of the edge (i,j,k).
 * @param[in] g Const reference to the DAG.
 * @returns std::pair<bool, VertexDesc> true if absorption takes place.
 *          If false, the VertexDesc is default-initialised.
 ******************************************************************************/
std::tuple<bool, bool, VertexDesc> check_absorption(
    const VertexDesc ij, const VertexDesc jk, const FaceDAG& g)
{
  bool exists = false;
  bool is_preacc = false;
  VertexDesc res = VertexDesc();

  ADM_DEBUG(eliminate_edge_detail_v) << "Absorbing or filling in result of "
                                     << ij << " -- " << jk << std::endl;

  // Loop over all predecessors of all successors of jk...
  BOOST_FOREACH(auto oe, out_edges(jk, g))
  {
    VertexDesc kl = target(oe, g);
    //  .. and all predecessors of all successors of jk.
    BOOST_FOREACH(auto ie, in_edges(kl, g))
    {
      VertexDesc ik = source(ie, g);
      // Skip if the predecessor of the successor is jk itself.
      if (jk == ik)
      {
        continue;
      }

      // (1.) Condition for generation of fill-in.
      // P(i,*,k) = P(i,*,j) && S(i,*,k) = S(j,*,k)
      if (same_predecessors(g, ij, ik) && same_successors(g, jk, ik))
      {
        if (get(boost::vertex_acc_stat, g, ik))
        {
          exists = true;
          is_preacc = true;
          res = ik;
        }
        else if (!exists && !is_preacc)
        {
          exists = true;
          res = ik;
        }
      }
    }
  }
  return std::make_tuple(exists, is_preacc, res);
}

/******************************************************************************
 * @brief Eliminates an edge \f$(i,j,k)\f$ from the face DAG g according
 *        to the generalised face-elimination algorithm (Rule 2).
 *
 * Wrapper taking an EdgeDesc and delegating to vertex-based function.
 *
 * @param[in] ijk EdgeDesc of the edge to eliminate.
 * @param[inout] g FaceDAG to eliminate ijk from.
 ******************************************************************************/
void eliminate_edge(const EdgeDesc ijk, FaceDAG& g)
{
  eliminate_edge(source(ijk, g), target(ijk, g), g);
}

/******************************************************************************
 * @brief Eliminates an edge \f$(i,j,k)\f$ from the face DAG g according
 *        to the generalised face-elimination algorithm (Rule 2).
 *
 * Face elimination is defined for all
 * \f$(i,j,k) = ((i,j),(j,k)) \in \tilde E_Z\f$.
 *
 * @param[in] ij VertexDesc of the source of \f$(i,j,k)\f$.
 * @param[in] jk VertexDesc of the target of \f$(i,j,k)\f$.
 * @param[inout] g  Reference to the face DAG.
 ******************************************************************************/
void eliminate_edge(const VertexDesc ij, const VertexDesc jk, FaceDAG& g)
{
#ifndef NDEBUG
  bool success;
  std::tie(std::ignore, success) = add_edge(ij, jk, g);
  assert(success == false && "The edge did not exist.");
  assert(
      in_degree(ij, g) && out_degree(jk, g) &&
      "Tried to eliminate a minimal/maximal face.");
#endif  // NDEBUG

  // Store the indices \f$(i,j,k)\f$ of the eliminated face as a property of
  // the face DAG.
  boost::set_property(g, boost::graph_previous_op, index_triple(ij, jk, g));

  // (1.)
  auto [exists, does_absorb, ik] = check_absorption(ij, jk, g);
  if (does_absorb)
  {
    absorb(ij, jk, ik, g);
  }
  else
  {
    fill_in(ij, jk, g);
  }

  // (2.)
  // We are not actually computing any Jacobians here.

  // (3.)
  remove_edge(ij, jk, g);

  // (4.)
  remove_or_merge(ij, g);

  // (5.)
  remove_or_merge(jk, g);
}

/******************************************************************************
 * @brief Performs a preaccumulation as used in the naive face
 *        elimination algorithm according to (Rule 3).
 *
 * Will merge the preaccumulated vertex with another preacc. vertex if possible.
 *
 * @param[in] ij VertexDesc of the vertex to accumulate.
 * @param[inout] g FaceDAG we operate on.
 * @param[in] remove_models bool, if true the adj and tan models are removed
 *                          from ij (default: false).
 ******************************************************************************/
void preaccumulate(const VertexDesc ij, FaceDAG& g, const bool remove_models)
{
  // Check if \f$\exists (i',j')\f$ such that \f$P(i',j') = P(i,j)\f$
  // and \f$S(i',j') = S(i,j)\f$ and \f$F'(i',j') != \emptyset\f$
  auto&& [has_candidate, iprimejprime] = has_merge_candidate(ij, g);
  if (has_candidate == true)
  {
    // (1. and 2.)
    // Merge (i',j') into (i,j).
    // Argument order is important here, as the second
    // argument is retained in the graph.
    merge_vertices(iprimejprime, ij, g);
  }
  else
  {
    // (1.)
    put(boost::vertex_acc_stat, g, ij, true);
    if (remove_models == true)
    {
      put(boost::vertex_has_model, g, ij, false);
    }
  }
}

/******************************************************************************
 * @brief Converts a DAG to have preaccumulated elemental
 *        Jacobians and no tangent or adjoint models.
 *
 * @param[inout] g Reference to the DAG.
 * @param[in] remove_models bool, if true the adj and tan models are removed
 *                          from ij (default: false).
 ******************************************************************************/
void preaccumulate_all(FaceDAG& g, bool remove_models)
{
  auto Fprime_exists = boost::get(boost::vertex_acc_stat, g);
  auto Fbardot_exists = boost::get(boost::vertex_has_model, g);

  BOOST_FOREACH(auto ij, vertices(g))
  {
    if (in_degree(ij, g) && out_degree(ij, g) && (!Fprime_exists[ij]) &&
        Fbardot_exists[ij])
    {
      preaccumulate(ij, g, remove_models);
    }
  }
}

}  // end namespace admission

// ################################## EOF ################################### //
