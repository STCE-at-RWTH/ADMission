#ifndef PIPE_PAIR_HPP
#define PIPE_PAIR_HPP

// ******************************** Includes ******************************** //

#include "graph/DAG.hpp"

#include <boost/type_traits/has_right_shift.hpp>

#include <istream>
#include <ostream>
#include <utility>

// **************************** Header contents ***************************** //

namespace boost
{

/**\brief Boost trait, enabling to use operator>> on a std::pair<index_t, index_t>.
 *
 *  Overload of a trait that was defined inside boost to determine,
 *  if operator>> is overloaded for a type.
 *  We explicitly enable it for std::pair<index_t, index_t>.
 *  @note Used by boost::read_graphml(...) and boost::write_graphml(...)
 */
template<>
class has_right_shift<
    std::basic_istream<wchar_t>,
    std::pair<admission::index_t, admission::index_t>> : public std::true_type
{};

}  // end namespace boost

namespace std
{

/** Overload for the << operator to append a std::pair<T, T> to a stream
 *  Calls operator<< on the both elements of type T of the std::pair<T, T>
 *  and separates them by a whitespace character.
 *  @param o Reference to the std::ostream to which to append.
 *  @param p Const reference to the std::pair<T, T> which is to be appended.
 *  @returns A reference to o.
 */
template<typename T>
std::ostream& operator<<(std::ostream& o, const std::pair<T, T>& p)
{
  o << p.first << " " << p.second;
  return o;
}

/** Overload of the >> operator to extract a std::pair<T, T> from a std::istream
 *  Uses operator>> to extract elements from the stream into both elements
 *  of the std::pair<T, T>.
 *  @param i Reference to the std::istream from which to extract.
 *  @param p Reference to the std::pair<T, T> in which the extracted elements are stored.
 *  @returns A reference to i.
 */
template<typename T>
std::istream& operator>>(std::istream& i, std::pair<T, T>& p)
{
  i >> p.first >> p.second;
  return i;
}

/**Printing a tuple recursively.
 */
template<int N, typename T>
std::enable_if_t<N != 0, void> tuple_print(std::ostream& o, const T& t)
{
  tuple_print<N - 1>(o, t);
  o << std::get<N>(t);
}

/**Termination case for recursive
 * tubple printing.
 */
template<int N, typename T>
std::enable_if_t<N == 0, void> tuple_print(std::ostream& o, const T& t)
{
  o << std::get<0>(t);
}

/**Implement printing of tuples.
 */
template<typename... Ts>
std::ostream& operator<<(std::ostream& o, const std::tuple<Ts...> t)
{
  tuple_print<sizeof...(Ts) - 1>(o, t);
  return o;
}

}  // end namespace std

#endif  // PIPE_PAIR_HPP
