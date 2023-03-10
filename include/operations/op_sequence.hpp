#ifndef ADM_INC_OPERATIONS_OP_SEQUENCE_HPP_
#define ADM_INC_OPERATIONS_OP_SEQUENCE_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "misc/doxygen.hpp"

#include <initializer_list>
#include <istream>
#include <limits>
#include <list>
#include <string>

// ########################## FORWARD DECLARATIONS ########################## //

namespace admission
{
class Op;
}

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(OpSequence)

/******************************************************************************
 * @brief Container for data members for accumulations and eliminations.
 *
 * We do this to have uniform data for AccOp and EliOp to store them in Arrays.
 * Declares essential apply functions that are forwarded to Op.
 ******************************************************************************/
struct OpCont
{
  //! Store a VertexDesc in case OpCont holds an AccOp.
  VertexDesc _ij;
  //! Store an EdgeDesc in case OpCont holds an EliOp.
  EdgeDesc _ijk;
  //! Store the indices in case OpCont holds a LazyEli/AccOp.
  index_t _i, _j, _k;
  //! Cost of the opaeration in terms of FMA.
  flop_t _cost;
  //! TAN or ADJ.
  dir_t _dir;
  //! Pointer to static (Lazy)Acc/EliOp to delegate function calls.
  Op* _functions;

  //! Delegate printing.
  ADM_ALWAYS_INLINE std::ostream& write(std::ostream& o) const;

  //! Delegate applycation to a face DAG.
  ADM_ALWAYS_INLINE bool apply(FaceDAG& g) const;

  //! Delegate printing of the index pair or triple.
  ADM_ALWAYS_INLINE const std::string where() const;
};

/******************************************************************************
 * @brief Base class for all operations.
 ******************************************************************************/
class Op
{
 public:
  //! Write this operation to a stream.
  virtual std::ostream& write(std::ostream&, const OpCont*) const = 0;

  //! Apply the Operation to a FaceDAG with certain properties for the
  //! EliminationAlgorithm.
  virtual bool apply(FaceDAG&, const OpCont*) const = 0;

  //! Return the descriptor of the edge or vertex respectively as a string.
  virtual const std::string where(const OpCont*) const = 0;
};

/******************************************************************************
 * @brief Global Operation, derives from Op.
 *
 * Implements the application and i/o functions for
 * eliminations. Derives from Op.
 ******************************************************************************/
class GlobOp : public Op
{
 public:
  //! Write info about this AccOp to a stream.
  virtual std::ostream& write(
      std::ostream& o, const OpCont* opc) const override final;

  //! Apply this EliOp to a FaceDAG.
  virtual bool apply(FaceDAG& g, const OpCont* opc) const override;

  //! Global operations do not print "where".
  virtual const std::string where(const OpCont* opc) const override final;
};

/******************************************************************************
 * @brief Accumulation Operation, derives from Op.
 *
 * Implements the application and i/o functions.
 ******************************************************************************/
class AccOp : public Op
{
 public:
  //! Write info about this AccOp to a stream.
  virtual std::ostream& write(
      std::ostream& o, const OpCont* opc) const override final;

  //! Apply this AccOp to a FaceDAG.
  virtual bool apply(FaceDAG& g, const OpCont* opc) const override;

  //! Stringify the indices _i and _j of an OpCont.
  virtual const std::string where(const OpCont* opc) const override final;
};

/******************************************************************************
 * @brief Lazy Accumulation Op that needs to search for
 *        the correct vertex before application.
 *
 * Used when reading Ops from a stream.
 * Derives its i/o functionsfrom AccOp.
 ******************************************************************************/
class LazyAccOp : public AccOp
{
 public:
  //! Apply this AccOp to a FaceDAG.
  virtual bool apply(FaceDAG& g, const OpCont* opc) const override final;
};

/******************************************************************************
 * @brief Elimination Operation.
 *
 * Implements the application and i/o functions for
 * eliminations. Derives from Op.
 ******************************************************************************/
class EliOp : public Op
{
 public:
  //! Write info about this EliOp to a stream.
  virtual std::ostream& write(
      std::ostream& o, const OpCont* opc) const override final;

  //! Apply this EliOp to a FaceDAG.
  virtual bool apply(FaceDAG& g, const OpCont* opc) const override;

  //! Stringify _i _j _k form the OpCont to i, j, k.
  virtual const std::string where(const OpCont* opc) const override final;
};

/******************************************************************************
 * @brief Lazy Elimination Operation that searches for the
 *        correct edge before eliminating it.
 *
 * Inherits its i/o functions from EliOp.
 ******************************************************************************/
class LazyEliOp : public EliOp
{
 public:
  //! Apply this EliOp to a FaceDAG.
  virtual bool apply(FaceDAG& g, const OpCont* opc) const override;
};

//! External make function for accumulation operation.
//! Convenient because of the explicit name.
OpCont make_acc_op(VertexDesc v, const FaceDAG& g, flop_t c, dir_t d);

//! External make function for accumulation operation.
//! Convenient because of the explicit name.
OpCont make_acc_op(index_t i, index_t j, flop_t c, dir_t d);

//! External make function for elimination operation.
//! Convenient because of the explicit name.
OpCont make_eli_op(EdgeDesc e, const FaceDAG& g, flop_t c, dir_t d);

//! External make function for elimination operation.
//! Convenient because of the explicit name.
OpCont make_eli_op(index_t i, index_t j, index_t k, flop_t c, dir_t d);

/******************************************************************************
 * @brief A Sequence of eliminations storing its overall cost.
 *
 * Contains a sequence of eliminations that can be applied to a FaceDAG.
 * Also stores the cost of the eliminations. This essentially wraps around a
 * std::list<Op*> and forwards its iterators etc. Additionally containts
 * functions to apply all eliminations to a FaceDAG either silent or verbose.
 ******************************************************************************/
class OpSequence
{
 public:
  //! Maximum cost value an OpSequence is initialised with.
  constexpr const static flop_t max = std::numeric_limits<flop_t>::max() / 2;

 protected:
  //! Constructor for OpSequence.
  ADM_ALWAYS_INLINE OpSequence(flop_t cost);

 public:
  //! Aggregate initialisation to explicitly create a sequence from a list of
  //! eliminations.
  ADM_ALWAYS_INLINE OpSequence(std::initializer_list<OpCont> init_list);

  //! Create an empty sequence with a cost of 0.
  ADM_ALWAYS_INLINE static OpSequence make_empty();

  //! Crate an empty sequence with a cost of max.
  ADM_ALWAYS_INLINE static OpSequence make_max();

  OpSequence() = delete;
  OpSequence(const OpSequence&) = default;
  OpSequence(OpSequence&&) = default;
  OpSequence& operator=(OpSequence&&) = default;
  OpSequence& operator=(const OpSequence&) = default;
  ~OpSequence() = default;

  // ------------------------------ Iterators ------------------------------- //
  DOXYGEN_GROUP_BEGIN(Iterators, "Forward iterators to stored _sequence.")

  //! Wrap around the begin() function of std::list.
  ADM_ALWAYS_INLINE auto begin() const;

  //! Wrap around the   end() function of std::list.
  ADM_ALWAYS_INLINE auto end() const;

  //! Wrap around the front() function of std::list.
  ADM_ALWAYS_INLINE auto& front() const;

  //! Wrap around the  back() function of std::list.
  ADM_ALWAYS_INLINE auto& back() const;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //

  // ------------------------------- Getters -------------------------------- //
  DOXYGEN_GROUP_BEGIN(Getters, "Query sizes and cost")

  //! Returns the cost of applying this OpSequence.
  ADM_ALWAYS_INLINE const flop_t& cost() const;

  //! Returns the number of Operation s in this sequence.
  ADM_ALWAYS_INLINE flop_t size() const;

  //! Returns true if the sequence is empty, false if not.
  ADM_ALWAYS_INLINE bool empty() const;

  DOXYGEN_GROUP_END()
  // ------------------------------------------------------------------------ //

  //! Concatenates two OpSequences and returns the result by value.
  ADM_ALWAYS_INLINE OpSequence operator+(const OpSequence& other) const;

  //! Appends another OpSequence to this and returns a *this as a reference.
  ADM_ALWAYS_INLINE OpSequence& operator+=(const OpSequence& other);

  //! Appends another OpSequence to this and returns a *this as a reference.
  ADM_ALWAYS_INLINE OpSequence& operator+=(OpSequence&& other);

  //! Applies the OpSequence to a FaceDAG.
  FaceDAG& apply(FaceDAG&) const;

  //! Applies the OpSequence to a FaceDAG and prints intermediate FaceDAG s.
  FaceDAG& verbose_apply(FaceDAG&, std::string path = "") const;

  //! Writes the OpSequence to a std::ostream.
  void write(std::ostream&) const;

 private:
  //! Cost op the application of the sequence.
  flop_t _cost;

  //! All operations.
  std::list<OpCont> _sequence;

  // Alternative Storage for the operations.
  // std::vector<OpCont> _sequence;
};

//! Read an OpSequence using the same syntax as the
//! admission::read_graph() functions.
void read_sequence(std::istream&& ist, OpSequence& els);

DOXYGEN_MODULE_END(OpSequence)

}  // end namespace admission

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "operations/impl/op_sequence.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_OPERATIONS_OP_SEQUENCE_HPP_
