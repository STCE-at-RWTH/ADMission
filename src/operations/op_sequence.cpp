// ******************************** Includes ******************************** //

#include "operations/op_sequence.hpp"

#include "graph/tikz.hpp"
#include "elimination_algorithm.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <assert.h>
#include <stddef.h>
#include <stdexcept>

// **************************** Source contents ***************************** //

namespace admission
{

/**\brief Write info about this AccOp to a stream.
 *
 * @param[inout] o Reference to a std::ostream.
 * @param[in] opc Pointer to the OpCont.
 */
std::ostream& AccOp::write(std::ostream& o, const OpCont* opc) const
{
  o << "PRE " << dir_to_s(opc->_dir) << " " << opc->_i << " " << opc->_j << " "
    << opc->_cost << std::endl;
  return o;
}

/**\brief Apply this AccOp to a FaceDAG.
 *
 * @param[inout] g Reference to the FaceDAG.
 * @param[in]  opc Pointer to the OpCont.
 * @returns bool if successful.
 */
bool AccOp::apply(FaceDAG& g, const OpCont* opc) const
{
  auto a = boost::get(boost::vertex_acc_stat, g);
  auto m = boost::get(boost::vertex_has_model, g);

  if (a[opc->_ij] == false && m[opc->_ij] == true)
  {
    Preaccumulate(opc->_ij, g);
    return true;
  }
  throw std::runtime_error(
      "The Jacobian F'_(" + opc->where() +
      ") is already preaccumulated or there is no model.");
}

/* \brief Applies the lazy accumulation operation by searching
 *        the DAG g for the correct vertex.
 *
 * @param[inout] g Reference to the DAG that is modified.
 * @param[in]  opc Pointer to the OpCont.
 */
bool LazyAccOp::apply(FaceDAG& g, const OpCont* opc) const
{
  auto a = boost::get(boost::vertex_acc_stat, g);
  auto m = boost::get(boost::vertex_has_model, g);

  BOOST_FOREACH(auto v, vertices(g))
  {
    if (in_degree(v, g) && out_degree(v, g) &&
        std::tie(opc->_i, opc->_j) == index_pair(v, g) && a[v] == false &&
        m[v] == true)
    {
      Preaccumulate(v, g);
      return true;
    }
  }
  throw std::runtime_error(
      "No elemental Jacobian F'_(" + opc->where() +
      ") to accumulate was found.");
}

/**\brief Stringify the indices _i and _j of an OpCont.
 *
 * @param opc Pointer to the OpCont.
 * @returns std::string (i, j)
 */
const std::string AccOp::where(const OpCont* opc) const
{
  return std::to_string(opc->_i) + ", " + std::to_string(opc->_j);
}

/**\brief Write info about this AccOp to a stream.
 *
 * @param[inout] o Reference to a std::ostream.
 * @param[in]  opc Pointer to the OpCont.
 * @returns std::ostream& o.
 */
std::ostream& EliOp::write(std::ostream& o, const OpCont* opc) const
{
  o << "ELI " << dir_to_s(opc->_dir) << " " << opc->_i << " " << opc->_j << " "
    << opc->_k << " " << opc->_cost << std::endl;
  return o;
}

/**\brief Apply this EliOp to a FaceDAG.
 *
 * @param[inout] g Reference to the FaceDAG.
 * @param[in]  opc Pointer to the OpCont.
 * @returns bool if successful.
 */
bool EliOp::apply(FaceDAG& g, const OpCont* opc) const
{
  admission::EliminateEdge(opc->_ijk, g);
  return true;
}

bool LazyEliOp::apply(FaceDAG& g, const OpCont* opc) const
{
  auto a = boost::get(boost::vertex_acc_stat, g);
  auto m = boost::get(boost::vertex_has_model, g);

  BOOST_FOREACH(auto e, edges(g))
  {
    if (in_degree(source(e, g), g) && out_degree(target(e, g), g) &&
        (index_triple(e, g) == std::tie(opc->_i, opc->_j, opc->_k)) &&
        (a[source(e, g)] || (opc->_dir == ADJ)) &&
        (a[target(e, g)] || (opc->_dir == TAN)) &&
        (m[source(e, g)] || !(opc->_dir == ADJ)) &&
        (m[target(e, g)] || !(opc->_dir == TAN)))
    {
      admission::EliminateEdge(e, g);
      return true;
    }
  }
  throw std::runtime_error(
      "No vertices with index triple (" + opc->where() +
      ") were found for operation " + dir_to_s(opc->_dir));
}

/**\brief Stringify _i _j _k form the OpCont to i, j, k.
 *
 * @param[in] opc Pointer to the OpCont.
 * @returns std::string (i, j, k)
 */
const std::string EliOp::where(const OpCont* opc) const
{
  return std::to_string(opc->_i) + ", " + std::to_string(opc->_j) + ", " +
         std::to_string(opc->_k);
}

/**\brief Read an OpSeqeunce from a stream.
 */
void read_sequence(std::istream&& ist, OpSequence& els)
{
  static std::string action;
  static std::string d;
  static flop_t c;
  static index_t i, j, k;

  ist >> action;
  while (!ist.eof())
  {
    if (action == "PRE")
    {
      ist >> d >> i >> j >> c;
      els += {make_acc_op(i, j, c, s_to_dir(d))};
    }
    else if (action == "ELI")
    {
      ist >> d >> i >> j >> k >> c;
      els += {make_eli_op(i, j, k, c, s_to_dir(d))};
    }
    else
    {
      throw std::runtime_error(
          "Selector \"" + action + "\" is neither \"ELI\" nor \"PRE\"!");
    }
    ist >> action;
  }
}

/**\brief Writes the elimination Sequence to a stream.
 */
void OpSequence::write(std::ostream& os) const
{
  for (auto& e : _sequence)
  {
    e.write(os);
  }
}

/**\brief Applies the stored sequence to g and prints the results into files.
 */
FaceDAG& OpSequence::verbose_apply(FaceDAG& g, std::string path) const
{
  // assign_vertex_indices(g);
  write_tikz(path + "graph0.xml", g);

  size_t k = 1;
  for (auto& e : _sequence)
  {
#ifndef NDEBUG
    auto success = e.apply(g);
    assert(success && "Failed to apply an elimination!");
#else
    e.apply(g);
#endif
    write_tikz(path + "graph" + std::to_string(k) + ".xml", g);
    k++;
  }
  return g;
}

/**\brief Applies the stored sequence to g and prints the results into files.
 */
FaceDAG& OpSequence::apply(FaceDAG& g) const
{
  for (auto& e : _sequence)
  {
#ifndef NDEBUG
    auto success = e.apply(g);
    assert(success && "Failed to apply an elimination!");
#else
    e.apply(g);
#endif
  }
  return g;
}

}  // end namespace admission
