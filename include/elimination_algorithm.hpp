#ifndef ELIMINATE_EDGE_HPP
#define ELIMINATE_EDGE_HPP

// ******************************** Includes ******************************** //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "graph/neighbors.hpp"
#include "operations/find_eliminations.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/pending/property.hpp>

#include <cassert>
#include <iostream>
#include <tuple>
#include <utility>

// **************************** Header contents ***************************** //

namespace admission
{

/**\brief Checks if a vertex that is part of an elimination
 *        will (can) be merged.
 *
 * @tparam Acc Type of the accessor functor to determine the direction in which we work.
 * @param[in] to_merge Source or target of the edge to be eliminated.
 * @param[in] to_ignore The other vertex of the edge to be eliminated.
 * @param[in] g Const reference to the DAG.
 * @returns bool bool VertexDesc First bool is true if a merge candidate exists,
 *                               Second bool is true if the candidate is accumulated,
 *                               VertexDesc is that vertex and otherwise default-initialised.
 */
template<typename Acc>
std::tuple<bool, bool, VertexDesc> check_merge(
    const VertexDesc to_merge, const VertexDesc to_ignore, const FaceDAG& g)
{
  bool exists = false;
  bool is_preacc = false;
  VertexDesc res = VertexDesc();

  // Loop over all successors (ik) of (to_merge)
  BOOST_FOREACH(auto oe, Acc::edges(to_merge, g))
  {
    VertexDesc jprimekprime = Acc::next_vertex(oe, g);

    // ... and over all predecessors (h,j) of all successors (j,k).
    BOOST_FOREACH(auto ie, Acc::reverse::edges(jprimekprime, g))
    {
      VertexDesc candidate = Acc::reverse::next_vertex(ie, g);

      if (candidate == to_merge)
      {
        continue;
      }

      // Check if succ. set and pred. set of j and v are identical
      // P(i,*,j) = P(*,j) && S(i,*,j) = S(*,j)
      if (same_neighbors_but_one<Acc>(g, candidate, to_merge, to_ignore) &&
          same_neighbors<FaceDAG, typename Acc::reverse>(
              g, candidate, to_merge))
      {
        // F'(*,j) != \emptyset
        if (get(boost::vertex_acc_stat, g, candidate))
        {
          exists = true;
          is_preacc = true;
          res = candidate;
        }
        else if (!is_preacc && !exists)
        {
          exists = true;
          res = candidate;
        }
      }
    }
  }
  return std::make_tuple(exists, is_preacc, res);
}

/**\brief (Rule 2)(1.) Checks if the the resulting vertex
 *        from eliminating (i,j,k) can be absorbed into
 *        another vertex (i,*,k).
 *
 * @param[in] ij VertexDesc of the source of the edge (i,j,k).
 * @param[in] jk VertexDesc of the target of the edge (i,j,k).
 * @returns std::pair<bool, VertexDesc> true if absorption takes place.
 *          If false, the VertexDesc is default-initialised.
 */
ADM_ALWAYS_INLINE
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

/** \brief Base class for algorithms that eliminate something,
 *         in particular EliminateEdge and MergeVertices.
 */
class EliminationAlgorithm
{
 protected:
  /**\brief Constructor.
   *
   * @param[inout] g The face DAG to modify.
   */
  EliminationAlgorithm(FaceDAG& g) : _g(g) {}

  ADM_ALWAYS_INLINE
  void merge_vertices(const VertexDesc from, const VertexDesc into);

  ADM_ALWAYS_INLINE
  void absorb(const VertexDesc ij, const VertexDesc jk, const VertexDesc ik);

  ADM_ALWAYS_INLINE
  void fill_in(const VertexDesc ij, const VertexDesc jk);

  ADM_ALWAYS_INLINE
  void remove_or_merge(const VertexDesc ij);

 protected:
  /// The face DAG w operate on.
  FaceDAG& _g;
};

/**\brief Eliminates an edge \f$(i,j,k)\f$ from the face DAG g according
 * to the generalised face-elimination algorithm (Rule 2).
 *
 * Face elimination is defined for all
 * \f$(i,j,k) = ((i,j),(j,k)) \in \tilde E_Z\f$:
 */
class EliminateEdge : public EliminationAlgorithm
{
 public:
  EliminateEdge(const EdgeDesc ijk, FaceDAG& g)
      : EliminateEdge(source(ijk, g), target(ijk, g), g)
  {}

  /**Executes the steps from (Rule 2).
   * @param[in] ij VertexDesc of the source of \f$(i,j,k)\f$.
   * @param[in] jk VertexDesc of the target of \f$(i,j,k)\f$.
   * @param[in] g  Reference to the face DAG.
   */
  EliminateEdge(const VertexDesc ij, const VertexDesc jk, FaceDAG& g)
      : EliminationAlgorithm(g)
  {
#ifndef NDEBUG
    bool success;
    std::tie(std::ignore, success) = add_edge(ij, jk, g);
    assert(success == false && "The edge did not exist.");
    assert(
        in_degree(ij, _g) && out_degree(jk, _g) &&
        "Tried to eliminate a minimal/maximal face.");
#endif  // NDEBUG

    // Store the indices \f$(i,j,k)\f$ of the eliminated face as a property of the face DAG.
    boost::set_property(_g, boost::graph_previous_op, index_triple(ij, jk, g));

    // (1.)
    auto [exists, does_absorb, ik] = check_absorption(ij, jk, g);
    if (does_absorb)
    {
      absorb(ij, jk, ik);
    }
    else
    {
      fill_in(ij, jk);
    }

    // (2.)
    // We are not actually computing any Jacobians here.

    // (3.)
    remove_edge(ij, jk, _g);

    // (4.)
    remove_or_merge(ij);

    // (5.)
    remove_or_merge(jk);
  }
};

/**\brief Performs a preaccumulation as used in the naive face
 *        elimination algorithm according to (Rule 3).
 *
 * Will merge the preaccumulated vertex with another preacc. vertex if possible.
 */
class Preaccumulate : public EliminationAlgorithm
{
 public:
  Preaccumulate(
      const VertexDesc ij, FaceDAG& g, const bool remove_models = false)
      : EliminationAlgorithm(g)
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
      merge_vertices(iprimejprime, ij);
    }
    else
    {
      // (1.)
      put(boost::vertex_acc_stat, _g, ij, true);
      if (remove_models == true)
      {
        put(boost::vertex_has_model, _g, ij, false);
      }
    }
  }
};

/**\brief Performs an absorption according to (Rule 2) (1.).
 *
 * @param ij VertexDesc of \f$(i,*,j)\f$.
 * @param jk VertexDesc of \f$(j,*,k)\f$.
 * @param ik VertexDesc of the absorbing vertex \f$(i,*,k)\f$.
 */
ADM_ALWAYS_INLINE
void EliminationAlgorithm::absorb(
    const VertexDesc ij, const VertexDesc jk, const VertexDesc ik)
{
  // Set c_tan(i,*,k) to zero.
  put(boost::vertex_c_tan, _g, ik, 0);
  // Set c_adj(i,*,k) to zero.
  put(boost::vertex_c_adj, _g, ik, 0);
  // F'(i,*,k) != \emptyset. Onlt relevant if F'(i,*,k) was preaccumulated above.
  put(boost::vertex_acc_stat, _g, ik, true);
  // \dot F(i,*,k) = \bar F(i,*,k) = \emptyset.
  put(boost::vertex_has_model, _g, ik, false);

  ADM_DEBUG(eliminate_edge_detail_v) << "Absorbing result of " << jk << " and "
                                     << ij << " into " << ik << std::endl;
}

/**\brief Generates a fill in (i,*,j,*,k) and connects it to the
 *        predecessors P(i,*,j) of (i,*,j) and the successors S(j,*,k)
 *        of (j,*,k) according to (Rule 2) (1.).
 *
 * @param[in] ij VertexDesc of (i,*,j).
 * @param[in] jk VertexDesc of (j,*,k).
 */
ADM_ALWAYS_INLINE
void EliminationAlgorithm::fill_in(const VertexDesc ij, const VertexDesc jk)
{
  ADM_DEBUG(eliminate_edge_detail_v)
      << "Generating fill-in as result of " << ij << " -- " << jk << std::endl;

  // Add (i,*,j,*,k) to g.
  VertexDesc ik = add_vertex(_g);

  // Add all required edges to ik such
  // that S(i,*,j,*,k) = S(j,*,k).
  BOOST_FOREACH(auto oe, out_edges(jk, _g))
  {
    auto new_e = add_edge(ik, target(oe, _g), _g);
    put(boost::edge_index, _g, new_e.first, get(boost::edge_index, _g, oe));
    put(boost::edge_size, _g, new_e.first, get(boost::edge_size, _g, oe));
  }

  // Add all required edges to ik such
  // that P(i,*,j,*,k) = P(i,*,j).
  BOOST_FOREACH(auto ie, in_edges(ij, _g))
  {
    auto new_e = add_edge(source(ie, _g), ik, _g);
    put(boost::edge_index, _g, new_e.first, get(boost::edge_index, _g, ie));
    put(boost::edge_size, _g, new_e.first, get(boost::edge_size, _g, ie));
  }

  // Set c_tan(i,*,j,*,k) to zero.
  put(boost::vertex_c_tan, _g, ik, 0);
  // Set c_adj(i,*,j,*,k) to zero.
  put(boost::vertex_c_adj, _g, ik, 0);
  // F'(i,*,j,*,k) != \emptyset.
  put(boost::vertex_acc_stat, _g, ik, true);
  // \dot F(i,*,j,*,k) = \bar F(i,*,j,*,k) = \emptyset.
  put(boost::vertex_has_model, _g, ik, false);
}

/**\brief Merges two vertices, i.e. just removes the "from" vertex.
 *        Corresponds to (Rule 2) (4.) (merge).
 *
 * @param from VertexDesc of the vertex to be merged.
 * @param into VertexDesc of the vertex into which the other one is merged.
 */
ADM_ALWAYS_INLINE
void EliminationAlgorithm::merge_vertices(
    const VertexDesc from, const VertexDesc into)
{
  ADM_DEBUG(eliminate_edge_detail_v)
      << "Merging vertex " << from << " into " << into << std::endl;

  // F'(i,*,j,*,k) != \emptyset.
  put(boost::vertex_has_model, _g, into, false);
  // \dot F(i,*,j,*,k) = \bar F(i,*,j,*,k) = \emptyset.
  put(boost::vertex_acc_stat, _g, into, true);

  // Isolate the vertex in both directions but not remove it,
  // saving us from copying elements of the vector storage
  // used by the FaceDAG class.
  clear_vertex(from, _g);
}

/**\brief (Rule 2) (4. and 5.) Removes a vertex (i,*,j) if it is bottom- or
 *        top-isolated. Tries to merge it with a parallel vertex otherwise.
 *
 * @param ij VertexDesc of the vertex (i,*,j) to remove or merge.
 * if a forced merge happens.
 */
ADM_ALWAYS_INLINE
void EliminationAlgorithm::remove_or_merge(const VertexDesc ij)
{
  ADM_DEBUG(eliminate_edge_detail_v)
      << "Trying to remove or merge vertex " << ij << std::endl;

  // If S(i,*,j) = \emptyset or P(i,*j) = \emptyset.
  // This covers the conditions from (Rule 2) (4. and 5.).
  if (in_degree(ij, _g) == 0 || out_degree(ij, _g) == 0)
  {
    // Isolate the vertex in both directions but not remove it,
    // saving us from copying elements of the vector storage
    // used by the FaceDAG class.
    clear_vertex(ij, _g);
    ADM_DEBUG(eliminate_edge_detail_v) << "Removed vertex " << ij << std::endl;
  }
  // ElseIf F'(i,*,j) = \emptyset. We do nothing.
  else if (get(boost::vertex_acc_stat, _g, ij) == false)
  {
    ADM_DEBUG(eliminate_edge_detail_v)
        << "Did not remove or merge vertex " << ij << std::endl;
  }
  // ElseIf F'(i,*,j) != \emptyset. We try to merge.
  else
  {
    // Iterate over all successors (j,k) of (i,j)
    BOOST_FOREACH(auto oe, out_edges(ij, _g))
    {
      VertexDesc jk = target(oe, _g);

      // ... and over all predecessors (h,j) of all successors (j,k).
      BOOST_FOREACH(auto ie, in_edges(jk, _g))
      {
        VertexDesc hj = source(ie, _g);
        if (hj == ij)
        {
          continue;
        }

        // Check if succ. set and pred. set of j and v are identical
        // P(i,*,j) = P(*,j) && S(i,*,j) = S(*,j) && F'(*,j) != \emptyset
        if (same_predecessors(_g, hj, ij) && same_successors(_g, hj, ij) &&
            get(boost::vertex_acc_stat, _g, hj))
        {
          merge_vertices(hj, ij);
          return;
        }
      }
    }
  }
  ADM_DEBUG(eliminate_edge_detail_v)
      << "Did not remove or merge vertex " << ij << std::endl;
}

/**\brief Converts a DAG to have preaccumulated elemental
 *        Jacobians and no tangent or adjoint models.
 *
 * @param[inout] g Reference to the DAG.
 * @returns OpSequence containing all preaccumulations.
 */
ADM_ALWAYS_INLINE
void preaccumulate_all(FaceDAG& g, bool remove_models = false)
{
  auto Fprime_exists = boost::get(boost::vertex_acc_stat, g);
  auto Fbardot_exists = boost::get(boost::vertex_has_model, g);

  BOOST_FOREACH(auto ij, vertices(g))
  {
    if (in_degree(ij, g) && out_degree(ij, g) && (!Fprime_exists[ij]) &&
        Fbardot_exists[ij])
    {
      Preaccumulate(ij, g, remove_models);
    }
  }
}

/**
 * @}
 */

}  // end namespace admission

#endif  // ELIMINATE_EDGE_HPP
