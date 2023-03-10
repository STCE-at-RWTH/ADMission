#ifndef ADM_INC_OPERATIONS_IMPL_OP_SEQUENCE_HPP_
#define ADM_INC_OPERATIONS_IMPL_OP_SEQUENCE_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "graph/DAG.hpp"
#include "operations/op_sequence.hpp"  // IWYU pragma: keep

#include <initializer_list>
#include <istream>
#include <list>
#include <string>
#include <utility>

// ###################### TEMPLATE AND INLINE CONTENTS ###################### //

namespace admission
{

/******************************************************************************
 * @brief Forward call to write() to the Op.
 *
 * Kept in header for inlining.
 *
 * @param[inout] o Reference to a std::ostream.
 * @returns std::ostream& o.
 ******************************************************************************/
ADM_ALWAYS_INLINE std::ostream& OpCont::write(std::ostream& o) const
{
  return _functions->write(o, this);
}

/******************************************************************************
 * @brief Forward call to apply() to the Op.
 *
 * Kept in header for inlining.
 *
 * @param[inout] g Reference to the FaceDAG.
 * @returns true if successful.
 ******************************************************************************/
ADM_ALWAYS_INLINE bool OpCont::apply(FaceDAG& g) const
{
  return _functions->apply(g, this);
}

/******************************************************************************
 * @brief Forward call to where() to the Op.
 *
 * Kept in header for inlining.
 *
 * @returns String representation of the Op.
 ******************************************************************************/
ADM_ALWAYS_INLINE const std::string OpCont::where() const
{
  return _functions->where(this);
}

/******************************************************************************
 * @brief Constructor for OpSequence.
 *
 * @param[in] cost Cost of the operation.
 ******************************************************************************/
ADM_ALWAYS_INLINE OpSequence::OpSequence(flop_t cost) : _cost(cost) {}

/******************************************************************************
 * @brief Aggregate initialisation to explicitly create a sequence from a
 *        list of eliminations.
 *
 * @param[in] init_list Initializer list.
 ******************************************************************************/
ADM_ALWAYS_INLINE OpSequence::OpSequence(std::initializer_list<OpCont> init_list)
    : _sequence(init_list)
{
  _cost = 0;
  for (auto& ep : _sequence)
  {
    _cost += ep._cost;
  }
}

/******************************************************************************
 * @brief Create an empty sequence with a cost of 0.
 *
 * @returns The OpSequence.
 ******************************************************************************/
ADM_ALWAYS_INLINE OpSequence OpSequence::make_empty()
{
  return OpSequence(0);
}

/******************************************************************************
 * @brief Crate an empty sequence with a cost of max.
 *
 * @returns The OpSequence.
 ******************************************************************************/
ADM_ALWAYS_INLINE OpSequence OpSequence::make_max()
{
  return OpSequence(OpSequence::max);
}

/******************************************************************************
 * @brief Wrap around the begin() function of std::list.
 *
 * @returns The iterator.
 ******************************************************************************/
ADM_ALWAYS_INLINE auto OpSequence::begin() const
{
  return _sequence.begin();
}

/******************************************************************************
 * @brief Wrap around the end() function of std::list.
 *
 * @returns The iterator.
 ******************************************************************************/
ADM_ALWAYS_INLINE auto OpSequence::end() const
{
  return _sequence.end();
}

/******************************************************************************
 * @brief Wrap around the front() function of std::list.
 *
 * @returns The iterator.
 ******************************************************************************/
ADM_ALWAYS_INLINE auto& OpSequence::front() const
{
  return _sequence.front();
}

/******************************************************************************
 * @brief Wrap around the back() function of std::list.
 *
 * @returns The iterator.
 ******************************************************************************/
ADM_ALWAYS_INLINE auto& OpSequence::back() const
{
  return _sequence.back();
}

/******************************************************************************
 * @brief Returns the cost of applying this OpSequence.
 *
 * @returns flop_t
 *******************************************************************************/
ADM_ALWAYS_INLINE const flop_t& OpSequence::cost() const
{
  return _cost;
}

/******************************************************************************
 * @brief Returns the number of Operation s in this sequence.
 *
 * @returns flop_t Length of the sequence.
 ******************************************************************************/
ADM_ALWAYS_INLINE flop_t OpSequence::size() const
{
  return _sequence.size();
}

/******************************************************************************
 * @brief Returns true if the sequence is empty, false if not.
 *
 * @returns bool
 ******************************************************************************/
ADM_ALWAYS_INLINE bool OpSequence::empty() const
{
  return _sequence.empty();
}

/******************************************************************************
 * @brief Concatenates two OpSequences and returns the result by value.
 *
 * @param[in] other The other operand.
 * @returns OpSequence The result of concatenation.
 ******************************************************************************/
ADM_ALWAYS_INLINE OpSequence OpSequence::operator+(const OpSequence& other) const
{
  OpSequence res = *this;
  res += other;
  return res;
}

/******************************************************************************
 * @brief Appends another OpSequence to this and returns a
 *        *this as a reference.
 *
 * @param[in] other The other operand.
 * @returns OpSequence& *this.
 ******************************************************************************/
ADM_ALWAYS_INLINE OpSequence& OpSequence::operator+=(const OpSequence& other)
{
  _cost += other._cost;
  for (auto& ep : other._sequence)
  {
    _sequence.emplace_back(ep);
  }
  return *this;
}

/******************************************************************************
 * @brief Appends another OpSequence to this and returns a
 *        *this as a reference.
 *
 * Allows fast concatenation by move-semantics.
 *
 * @param[in] other OpSequence that can be reused.
 * @returns OpSequence& *this.
 ******************************************************************************/
ADM_ALWAYS_INLINE OpSequence& OpSequence::operator+=(OpSequence&& other)
{
  // Vector storage.
  // _sequence.insert(_sequence.end(), other._sequence.begin(),
  // other._sequence.end());

  // List storage.
  _sequence.splice(_sequence.end(), std::move(other._sequence));
  _cost += other._cost;
  return *this;
}

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_OPERATIONS_IMPL_OP_SEQUENCE_HPP_
