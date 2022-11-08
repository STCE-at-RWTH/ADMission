#ifndef OP_SEQUENCE_HPP
#define OP_SEQUENCE_HPP

// ******************************** Includes ******************************** //

#include "admission_config.hpp"
#include "graph/DAG.hpp"

#include <initializer_list>
#include <istream>
#include <limits>
#include <list>
#include <string>
#include <tuple>
#include <utility>

// ************************** Forward declarations ************************** //

namespace admission
{
class Op;
}

// **************************** Header contents ***************************** //

namespace admission
{

/**
 * \defgroup OpSequence Storing and applying Accumulations end Eliminations in sequences.
 * \addtogroup OpSequence
 * @{
 */

/** \brief Convert an internal enum value to string for i/o.
 *
 *  @param o enum value of the operation
 *  @returns The corresponding string.
 */
ADM_ALWAYS_INLINE
const std::string dir_to_s(dir_t o)
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

/** \brief Convert a string to internal enum value for i/o.
 *
 *  @param s identifier string.
 *  @returns The corresponding enum value.
 */
ADM_ALWAYS_INLINE
dir_t s_to_dir(std::string s)
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

/**
 * \brief Container for data members for accumulations and eliminations.
 *
 * We do this to have uniform data for AccOp and EliOp to store them in Arrays.
 * Declares essential apply functions that are forwarded to Op.
 */
struct OpCont
{
  /// Store a VertexDesc in case OpCont holds an AccOp.
  VertexDesc _ij;
  /// Store an EdgeDesc in case OpCont holds an EliOp.
  EdgeDesc _ijk;
  /// Store the indices in case OpCont holds a LazyEli/AccOp.
  index_t _i, _j, _k;
  /// Cost of the opaeration in terms of FMA.
  flop_t _cost;
  /// TAN or ADJ.
  dir_t _dir;
  /// Pointer to static (Lazy)Acc/EliOp to delegate function calls.
  Op* _functions;

  /// Delegate printing.
  std::ostream& write(std::ostream& o) const;
  /// Delegate applycation to a face DAG.
  bool apply(FaceDAG& g) const;
  /// Delegate printing of the index pair or triple.
  const std::string where() const;
};

class Op
{
 public:
  /**\brief Write this operation to a stream.
   */
  virtual std::ostream& write(std::ostream&, const OpCont*) const = 0;

  /**\brief Apply the Operation to a FaceDAG with certain properties for the EliminationAlgorithm.
   */
  virtual bool apply(FaceDAG&, const OpCont*) const = 0;

  /**\brief Return the descriptor of the edge or vertex respectively as a string.
   */
  virtual const std::string where(const OpCont*) const = 0;
};

/**
 * \brief Accumulation Operation, derives from Op.
 *
 * Implements the application and i/o functions.
 */
class AccOp : public Op
{
 public:
  /**\brief Write info about this AccOp to a stream.
   *
   * @param[inout] o Reference to a std::ostream.
   * @param [in]  opc Pointer to the container.
   */
  virtual std::ostream& write(
      std::ostream& o, const OpCont* opc) const override final;

  /**\brief Apply this AccOp to a FaceDAG.
   *
   * @param [inout] g Reference to the FaceDAG.
   * @param [in]  opc Pointer to the container.
   * @returns bool true if successful.
   */
  virtual bool apply(FaceDAG& g, const OpCont* opc) const override;

  /**\brief Stringify the index pair _where.
   *
   * @param [in]  opc Pointer to the container.
   * @returns std::string "i, j"
   */
  virtual const std::string where(const OpCont* opc) const override;
};

/**\brief Lazy Accumulation Op that needs to search for
 *        the correct vertex before application.
 *
 *        Used when reading Ops from a stream.
 *        Derives its i/o functionsfrom AccOp.
 */
class LazyAccOp : public AccOp
{
 public:
  /**\brief Apply this AccOp to a FaceDAG.
   *
   * @param [inout] g Reference to the FaceDAG.
   * @param [in]  opc Pointer to the container.
   * @returns bool true if successful.
   */
  virtual bool apply(FaceDAG& g, const OpCont* opc) const override;
};

/**\brief Elimination Operation.
 *
 * Implements the application and i/o functions for
 * eliminations. Derives from Op.
 */
class EliOp : public Op
{
 public:
  /**\brief Write info about this AccOp to a stream.
   *
   * @param[inout] o Reference to a std::ostream.
   * @param [in]  opc Pointer to the container.
   * @returns std::ostream& o.
   */
  virtual std::ostream& write(
      std::ostream& o, const OpCont* opc) const override final;

  /**\brief Apply this EliOp to a FaceDAG.
   *
   * @param [inout] g Reference to the FaceDAG.
   * @param [in]  opc Pointer to the container.
   * @returns bool true if successful.
   */
  virtual bool apply(FaceDAG& g, const OpCont* opc) const override;

  /**\brief Stringify the index triple _where.
   *
   * @param [in]  opc Pointer to the container.
   * @returns std::string (i, j, k)
   */
  virtual const std::string where(const OpCont* opc) const override final;
};

/**\brief Lazy Elimination Operation that searches for the
 *        correct edge before eliminating it.
 *
 *        Inherits its i/o functions from EliOp.
 */
class LazyEliOp : public EliOp
{
 public:
  /**\brief Apply this EliOp to a FaceDAG.
   *
   * @param [inout] g Reference to the FaceDAG.
   * @param [in]  opc Pointer to the container.
   * @returns bool true if successful.
   */
  virtual bool apply(FaceDAG& g, const OpCont* opc) const override;
};

/** Forward call to write() to the Op. Kept in header for inlining.
 */
inline std::ostream& OpCont::write(std::ostream& o) const
{
  return _functions->write(o, this);
}

/** Forward call to apply() to the Op. Kept in header for inlining.
 */
inline bool OpCont::apply(FaceDAG& g) const
{
  return _functions->apply(g, this);
}

/** Forward call to where() to the Op. Kept in header for inlining.
 */
inline const std::string OpCont::where() const
{
  return _functions->where(this);
}

/*
 * We avoid possible overhead
 * if the construction of Op
 * that does not contain data
 * members is not optimized out.
 */

/// Static AccOp to point to in OpCont
static AccOp acc_op;
/// Static EliOp to point to in OpCont
static EliOp eli_op;
/// Static LazyAccOp to point to in OpCont
static LazyAccOp lazy_acc_op;
/// Static LazyEliOp to point to in OpCont
static LazyEliOp lazy_eli_op;

/**\brief External make function for accumulation operation.
 *        Convenient because of the explicit name.
 *
 * We could also do this as a constructor of OpCont,
 * but would have a more implicit syntax.
 * @param[in] v VertexDesc of the vertex to be accumulated.
 * @param[in] g const & to the FaceDAG.
 * @param[in] c cost of the accumulation to be stored.
 * @param[in] d direction TAN or ADJ.
 * @returns OpCont whose _functions point to acc_op.
 */
inline OpCont make_acc_op(VertexDesc v, const FaceDAG& g, flop_t c, dir_t d)
{
  OpCont opc;
  opc._ij = v;
  std::tie(opc._i, opc._j) = index_pair(v, g);
  opc._cost = c;
  opc._dir = d;
  opc._functions = &acc_op;
  return opc;
}

/**\brief External make function for accumulation operation.
 *        Convenient because of the explicit name.
 *
 * We could also do this as a constructor of OpCont,
 * but would have a more implicit syntax.
 * This version leaves _ij uninitilised and sets _functions
 * to lazy_acc_op.
 * @param[in] i First index of the vertex ij.
 * @param[in] j Second index of the vertex ij.
 * @param[in] g const & to the FaceDAG.
 * @param[in] c cost of the accumulation to be stored.
 * @param[in] d direction TAN or ADJ.
 * @returns OpCont whose _functions point to acc_op.
 */
inline OpCont make_acc_op(index_t i, index_t j, flop_t c, dir_t d)
{
  OpCont opc;
  opc._i = i;
  opc._j = j;
  opc._dir = d;
  opc._cost = c;
  opc._functions = &lazy_acc_op;
  return opc;
}

/**\brief External make function for elimination operation.
 *        Convenient because of the explicit name.
 *
 * We could also do this as a constructor of OpCont,
 * but would have a more implicit syntax.
 * @param[in] e EdgeDesc of the edge to be eliminated.
 * @param[in] g const & to the FaceDAG.
 * @param[in] c cost of the accumulation to be stored.
 * @param[in] d direction TAN or ADJ.
 * @returns OpCont whose _functions point to acc_op.
 */
inline OpCont make_eli_op(EdgeDesc e, const FaceDAG& g, flop_t c, dir_t d)
{
  OpCont opc;
  opc._ijk = e;
  std::tie(opc._i, opc._j, opc._k) = index_triple(e, g);
  opc._dir = d;
  opc._cost = c;
  opc._functions = &eli_op;
  return opc;
}

/**\brief External make function for elimination operation.
 *        Convenient because of the explicit name.
 *
 * We could also do this as a constructor of OpCont,
 * but would have a more implicit syntax.
 * This version leaves _ijk uninitilised and sets _functions
 * to lazy_eli_op.
 * @param[in] i First index of the vertex (i,j,k).
 * @param[in] j Second index of the vertex (i,j,k).
 * @param[in] k Second index of the vertex (i,j,k).
 * @param[in] g const & to the FaceDAG.
 * @param[in] c cost of the accumulation to be stored.
 * @param[in] d direction TAN or ADJ.
 * @returns OpCont whose _functions point to acc_op.
 */
inline OpCont make_eli_op(index_t i, index_t j, index_t k, flop_t c, dir_t d)
{
  OpCont opc;
  opc._i = i;
  opc._j = j;
  opc._k = k;
  opc._dir = d;
  opc._cost = c;
  opc._functions = &lazy_eli_op;
  return opc;
}

/**\brief A Sequence of eliminations storing its overall cost.
 *
 * Contains a sequence of eliminations that can be applied to a FaceDAG.
 * Also stores the cost of the eliminations. This essentially wraps around a
 * std::list<Op*> and forwards its iterators etc. Additionally containts functions
 * to apply all eliminations to a FaceDAG either silent or verbose.
 */
class OpSequence
{
 public:
  /// Maximum cost value an OpSequence is initialised with.
  constexpr const static flop_t max = std::numeric_limits<flop_t>::max() / 2;

  /**\name Constructors and destructors.
   *
   * Also contains explicit make_... functions.
   */
  ///@{
 protected:
  OpSequence(flop_t cost) : _cost(cost) {}

 public:
  /// Create an empty sequence with a cost of 0.
  ADM_ALWAYS_INLINE
  static OpSequence make_empty()
  {
    return OpSequence(0);
  }

  /// Crate an empty sequence with a cost of max.
  ADM_ALWAYS_INLINE
  static OpSequence make_max()
  {
    return OpSequence(OpSequence::max);
  }

  OpSequence() = delete;
  OpSequence(const OpSequence&) = default;
  OpSequence(OpSequence&&) = default;
  OpSequence& operator=(OpSequence&&) = default;
  OpSequence& operator=(const OpSequence&) = default;
  ~OpSequence() = default;
  ///@}

 public:
  /**\name Iterators.
   *
   * Forward iterators and references to stored _sequence.
   */
  ///@{

  /// Wrap around the begin() function of std::list.
  ADM_ALWAYS_INLINE
  auto begin() const
  {
    return _sequence.begin();
  }

  /// Wrap around the   end() function of std::list.
  ADM_ALWAYS_INLINE
  auto end() const
  {
    return _sequence.end();
  }

  /// Wrap around the front() function of std::list.
  ADM_ALWAYS_INLINE
  auto& front() const
  {
    return _sequence.front();
  }

  /// Wrap around the  back() function of std::list.
  ADM_ALWAYS_INLINE
  auto& back() const
  {
    return _sequence.back();
  }

  ///@}

  /// Aggregate initialisation to explicitly create a sequence from a list of eliminations.
  ADM_ALWAYS_INLINE
  OpSequence(std::initializer_list<OpCont> init_list) : _sequence(init_list)
  {
    _cost = 0;
    for (auto& ep : _sequence)
    {
      _cost += ep._cost;
    }
  }

  /**\name Getters.
   *
   * Query sizes and cost.
   */
  ///@{

  /// Returns the cost of applying this OpSequence.
  ADM_ALWAYS_INLINE
  const flop_t& cost() const
  {
    return _cost;
  }

  /// Returns the number of Operation s in this sequence.
  ADM_ALWAYS_INLINE
  flop_t size() const
  {
    return _sequence.size();
  }

  /// Returns true if the sequence is empty, false if not.
  ADM_ALWAYS_INLINE
  bool empty() const
  {
    return _sequence.empty();
  }

  ///@}

  /**\brief Concatenates two OpSequences
   *        and returns the result by value.
   *
   * @param[in] other The other operand.
   * @returns OpSequence The result of concatenation.
   */
  ADM_ALWAYS_INLINE
  OpSequence operator+(const OpSequence& other) const
  {
    OpSequence res = *this;
    res += other;
    return res;
  }

  /**\brief Appends another OpSequence to this
   *        and returns a *this as a reference.
   *
   * @param[in] other The other operand.
   * @returns OpSequence& *this.
   */
  ADM_ALWAYS_INLINE
  OpSequence& operator+=(const OpSequence& other)
  {
    _cost += other._cost;
    for (auto& ep : other._sequence)
    {
      _sequence.emplace_back(ep);
    }
    return *this;
  }

  /**\brief Appends another OpSequence to this
   *        and returns a *this as a reference.
   *
   * Allows fast concatenation by move-semantics.
   * @param[in] other OpSequence that can be reused.
   * @returns OpSequence& *this.
   */
  ADM_ALWAYS_INLINE
  OpSequence& operator+=(OpSequence&& other)
  {
    // Vector storage.
    // _sequence.insert(_sequence.end(), other._sequence.begin(), other._sequence.end());

    // List storage.
    _sequence.splice(_sequence.end(), std::move(other._sequence));
    _cost += other._cost;
    return *this;
  }

  /// Applies the OpSequence to a FaceDAG.
  FaceDAG& apply(FaceDAG&) const;

  /// Applies the OpSequence to a FaceDAG and prints intermediate FaceDAG s.
  FaceDAG& verbose_apply(FaceDAG&, std::string path = "") const;

  /// Writes the OpSequence to a std::ostream.
  void write(std::ostream&) const;

 private:
  /// Cost op the application of the sequence.
  flop_t _cost;

  /// All operations.
  std::list<OpCont> _sequence;

  /// Alternative Storage for the operations.
  // std::vector<OpCont> _sequence;
};

/** \brief Read an OpSequence using the same syntax as the admissio::read_graph() functions.
 * @param[in]  ist std::istream to read the OpSequence from.
 * @param[out] els OpSequence& to fill.
 */
void read_sequence(std::istream&& ist, OpSequence& els);

/**
 * @}
 */

}  // end namespace admission

#endif  // ELIM_SEQUENCE_HPP
