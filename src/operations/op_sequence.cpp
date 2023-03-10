// ################################ INCLUDES ################################ //

#include "operations/op_sequence.hpp"

#include "graph/tikz.hpp"
#include "operations/elimination_algorithms.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <stddef.h>
#include <stdexcept>
#include <tuple>

// ############################ SOURCE CONTENTS ############################# //

namespace admission
{

// --------------------------- Static helpers ------------------------------- //

/******************************************************************************
 * @brief Convert an internal enum value to string for i/o.
 *
 * @param[in] o enum value of the operation
 * @returns The corresponding string.
 ******************************************************************************/
static const std::string dir_to_s(dir_t o)
{
  switch (o)
  {
    case MUL:
    {
      return "MUL";
    }
    case TAN:
    {
      return "TAN";
    }
    case ADJ:
    {
      return "ADJ";
    }
    default:
    {
      throw;
    }
  }
}

/******************************************************************************
 * @brief Convert a string to internal enum value for i/o.
 *
 * @param[in] s identifier string.
 * @returns The corresponding enum value.
 ******************************************************************************/
static dir_t s_to_dir(std::string s)
{
  if (s == "MUL")
  {
    return MUL;
  }
  else if (s == "TAN")
  {
    return TAN;
  }
  else if (s == "ADJ")
  {
    return ADJ;
  }
  else
  {
    throw;
  }
}

// --------------------------------- AccOp ---------------------------------- //

/******************************************************************************
 * @brief Write info about this AccOp to a stream.
 *
 * @param[inout] o Reference to a std::ostream.
 * @param[in] opc Pointer to the OpCont.
 * @returns std::ostream& o.
 ******************************************************************************/
std::ostream& AccOp::write(std::ostream& o, const OpCont* opc) const
{
  o << "  ACC " << dir_to_s(opc->_dir) << " (" << opc->_i << " " << opc->_j
    << ") " << opc->_cost << std::endl;
  return o;
}

/******************************************************************************
 * @brief Apply this AccOp to a FaceDAG.
 *
 * @param[inout] g Reference to the FaceDAG.
 * @param[in] opc Pointer to the OpCont.
 * @returns true if successful.
 ******************************************************************************/
bool AccOp::apply(FaceDAG& g, const OpCont* opc) const
{
  auto a = boost::get(boost::vertex_acc_stat, g);
  auto m = boost::get(boost::vertex_has_model, g);

  if (a[opc->_ij] == false && m[opc->_ij] == true)
  {
    preaccumulate(opc->_ij, g);
    return true;
  }
  throw std::runtime_error(
      "The Jacobian F'_(" + opc->where() +
      ") is already preaccumulated or there is no model.");
}

/******************************************************************************
 * @brief Stringify the indices _i and _j of an OpCont.
 *
 * @param[in] opc Pointer to the OpCont.
 * @returns std::string (i, j)
 *******************************************************************************/
const std::string AccOp::where(const OpCont* opc) const
{
  return std::to_string(opc->_i) + ", " + std::to_string(opc->_j);
}

// ------------------------------- LazyAccOp -------------------------------- //

/******************************************************************************
 * @brief Applies the lazy accumulation operation by searching
 *        the DAG g for the correct vertex.
 *
 * @param[inout] g Reference to the DAG that is modified.
 * @param[in] opc Pointer to the OpCont.
 * @returns true if successful.
 ******************************************************************************/
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
      preaccumulate(v, g);
      return true;
    }
  }
  throw std::runtime_error(
      "No elemental Jacobian F'_(" + opc->where() +
      ") to accumulate was found.");
}

// --------------------------------- EliOp ---------------------------------- //

/******************************************************************************
 * @brief Write info about this EliOp to a stream.
 *
 * @param[inout] o Reference to a std::ostream.
 * @param[in]  opc Pointer to the OpCont.
 * @returns std::ostream& o.
 *******************************************************************************/
std::ostream& EliOp::write(std::ostream& o, const OpCont* opc) const
{
  o << "  ELI " << dir_to_s(opc->_dir) << " (" << opc->_i << " " << opc->_j
    << " " << opc->_k << ") " << opc->_cost << std::endl;
  return o;
}

/******************************************************************************
 * @brief Apply this EliOp to a FaceDAG.
 *
 * @param[inout] g Reference to the FaceDAG.
 * @param[in] opc Pointer to the OpCont.
 * @returns bool if successful.
 ******************************************************************************/
bool EliOp::apply(FaceDAG& g, const OpCont* opc) const
{
  admission::eliminate_edge(opc->_ijk, g);
  return true;
}

/******************************************************************************
 * @brief Stringify _i _j _k form the OpCont to i, j, k.
 *
 * @param[in] opc Pointer to the OpCont.
 * @returns std::string (i, j, k)
 ******************************************************************************/
const std::string EliOp::where(const OpCont* opc) const
{
  return std::to_string(opc->_i) + ", " + std::to_string(opc->_j) + ", " +
         std::to_string(opc->_k);
}

// ------------------------------- LazyEliOp -------------------------------- //

/******************************************************************************
 * @brief Apply this EliOp to a FaceDAG.
 *
 * @param[inout] g Reference to the FaceDAG.
 * @param[in] opc  Pointer to the container.
 * @returns true if successful.
 ******************************************************************************/
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
      admission::eliminate_edge(e, g);
      return true;
    }
  }
  throw std::runtime_error(
      "No vertices with index triple (" + opc->where() +
      ") were found for operation " + dir_to_s(opc->_dir));
}

// ------------------------------ Op creation ------------------------------- //

/******************************************************************************
 * @brief External make function for accumulation operation.
 *        Convenient because of the explicit name.
 *
 * We could also do this as a constructor of OpCont,
 * but would have a more implicit syntax.
 *
 * @param[in] v VertexDesc of the vertex to be accumulated.
 * @param[in] g const & to the FaceDAG.
 * @param[in] c cost of the accumulation to be stored.
 * @param[in] d direction TAN or ADJ.
 * @returns OpCont whose _functions point to acc_op.
 ******************************************************************************/
OpCont make_acc_op(VertexDesc v, const FaceDAG& g, flop_t c, dir_t d)
{
  // Static AccOp to point to in OpCont
  static AccOp acc_op;

  OpCont opc;
  opc._ij = v;
  std::tie(opc._i, opc._j) = index_pair(v, g);
  opc._cost = c;
  opc._dir = d;
  opc._functions = &acc_op;
  return opc;
}

/******************************************************************************
 * @brief External make function for accumulation operation.
 *        Convenient because of the explicit name.
 *
 * We could also do this as a constructor of OpCont, but would have a more
 * implicit syntax. This version leaves _ij uninitilised and sets _functions
 * to lazy_acc_op.
 *
 * @param[in] i First index of the vertex ij.
 * @param[in] j Second index of the vertex ij.
 * @param[in] g const & to the FaceDAG.
 * @param[in] c cost of the accumulation to be stored.
 * @param[in] d direction TAN or ADJ.
 * @returns OpCont whose _functions point to acc_op.
 ******************************************************************************/
OpCont make_acc_op(index_t i, index_t j, flop_t c, dir_t d)
{
  // Static LazyAccOp to point to in OpCont
  static LazyAccOp lazy_acc_op;

  OpCont opc;
  opc._i = i;
  opc._j = j;
  opc._dir = d;
  opc._cost = c;
  opc._functions = &lazy_acc_op;
  return opc;
}

/******************************************************************************
 * @brief External make function for elimination operation.
 *        Convenient because of the explicit name.
 *
 * We could also do this as a constructor of OpCont, but would have a more
 * implicit syntax.
 *
 * @param[in] e EdgeDesc of the edge to be eliminated.
 * @param[in] g const & to the FaceDAG.
 * @param[in] c cost of the accumulation to be stored.
 * @param[in] d direction TAN or ADJ.
 * @returns OpCont whose _functions point to acc_op.
 ******************************************************************************/
OpCont make_eli_op(EdgeDesc e, const FaceDAG& g, flop_t c, dir_t d)
{
  // Static EliOp to point to in OpCont
  static EliOp eli_op;

  OpCont opc;
  opc._ijk = e;
  std::tie(opc._i, opc._j, opc._k) = index_triple(e, g);
  opc._dir = d;
  opc._cost = c;
  opc._functions = &eli_op;
  return opc;
}

/******************************************************************************
 * @brief External make function for elimination operation.
 *        Convenient because of the explicit name.
 *
 * We could also do this as a constructor of OpCont, but would have a more
 * implicit syntax. This version leaves _ijk uninitilised and sets _functions
 * to lazy_eli_op.
 *
 * @param[in] i First index of the vertex (i,j,k).
 * @param[in] j Second index of the vertex (i,j,k).
 * @param[in] k Second index of the vertex (i,j,k).
 * @param[in] g const & to the FaceDAG.
 * @param[in] c cost of the accumulation to be stored.
 * @param[in] d direction TAN or ADJ.
 * @returns OpCont whose _functions point to acc_op.
 ******************************************************************************/
OpCont make_eli_op(index_t i, index_t j, index_t k, flop_t c, dir_t d)
{
  // Static LazyEliOp to point to in OpCont
  static LazyEliOp lazy_eli_op;

  OpCont opc;
  opc._i = i;
  opc._j = j;
  opc._k = k;
  opc._dir = d;
  opc._cost = c;
  opc._functions = &lazy_eli_op;
  return opc;
}

// ------------------------------- OpSequence ------------------------------- //

/******************************************************************************
 * @brief Applies the stored sequence to g.
 ******************************************************************************/
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

/******************************************************************************
 * @brief Applies the stored sequence to g and prints the results into files.
 ******************************************************************************/
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

/******************************************************************************
 * @brief Writes the elimination Sequence to a stream.
 ******************************************************************************/
void OpSequence::write(std::ostream& os) const
{
  for (auto& e : _sequence)
  {
    e.write(os);
  }
}

/******************************************************************************
 * @brief Read an OpSequence using the same syntax as the
 *        admission::read_graph() functions.
 *
 * @param[in] ist Input stream to read from.
 * @param[inout] els Elimination sequence constructed from the input.
 ******************************************************************************/
void read_sequence(std::istream&& ist, OpSequence& els)
{
  static std::string action;
  static std::string d;
  static flop_t c;
  static index_t i, j, k;

  ist >> action;
  while (!ist.eof())
  {
    if (action == "ACC")
    {
      ist >> d >> i >> j >> c;
      els += {make_acc_op(i, j, c, s_to_dir(d))};
    }
    else if (action == "ACC")
    {
      ist >> d >> i >> j >> k >> c;
      els += {make_eli_op(i, j, k, c, s_to_dir(d))};
    }
    else
    {
      throw std::runtime_error(
          "Selector \"" + action + "\" is neither \"ELI\" nor \"ACC\"!");
    }
    ist >> action;
  }
}

}  // end namespace admission

// ################################## EOF ################################### //
