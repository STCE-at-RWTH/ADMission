#ifndef GRAPH_GENERATOR_HPP
#define GRAPH_GENERATOR_HPP

// ******************************** Includes ******************************** //

#include "admission_config.hpp"
#include "properties.hpp"

#include <boost/graph/adjacency_list.hpp>

#include <algorithm>
#include <cmath>
#include <random>
#include <stddef.h>
#include <vector>

// **************************** Header contents ***************************** //

namespace admission
{

/**\brief Stores all relevant properties for generating a "random" DAG.
 *
 * \note See calls to the register_property(...)
 * functions for explanation of the parameters.
 */
class GraphGeneratorProperties : public Properties
{
 public:
  GraphGeneratorProperties()
  {
    register_property(
        num_vertices, "num_vertices", "Number of vertices in the DAG.");
    register_property(
        num_intermediate_vertices, "num_intermediate_vertices",
        "Number of intermediate vertices in the DAG. Must be smaller than "
        "NumVertices-2.");
    register_property(
        coeff_const, "coeff_const",
        "Parameter of the probability density function of the distance spanned "
        "by an edge. Const distribution.");
    register_property(
        coeff_ascending, "coeff_ascending",
        "Parameter of the probability density function of the distance spanned "
        "by an edge. Ascending probability.");
    register_property(
        coeff_descending, "coeff_descending",
        "Parameter of the probability density function of the distance spanned "
        "by an edge. Descending probability.");
    register_property(
        coeff_sin, "coeff_sin",
        "Parameter of the probability density function of the distance spanned "
        "by an edge. Modified sine wave probability.");
    register_property(
        max_vertex_size, "max_vertex_size",
        "Max vector size of a vertex. Actual size is randomly chosen in [1, "
        "MaxVertexSize].");
    register_property(
        max_in_out, "max_in_out",
        "Maximal desired number of in-edges and out-edges of any vertex. "
        "Actual number of in-edges of intermediate vertex i may be in [1, "
        "i-1].");
    register_property(
        cost_scaling, "cost_scaling",
        "Scale the max value for the random cost generator relatively to "
        "vertex sizes.");
  }

  size_t num_vertices = 0;
  size_t num_intermediate_vertices = 0;
  double coeff_const = 1.0;
  double coeff_ascending = 0.0;
  double coeff_descending = 0.0;
  double coeff_sin = 0.0;
  size_t max_vertex_size = 1;
  size_t max_in_out = 100;
  flop_t cost_scaling = 1;
};

/**\brief Generates a pseudo-random DAG.
 *
 * Generates a pseudo-random DAG ensuring certain properties
 * like number of vertices and connectivity.
 */
template<typename Graph>
class GraphGenerator
{
  /// A uniform distribution for predecessor- and successor numbers.
  template<typename T = size_t>
  using dist_t = std::uniform_int_distribution<T>;

  /// A weighted distribuiton for edge generation.
  template<typename T = size_t>
  using weighted_dist_t = std::discrete_distribution<T>;

 protected:
  /**\brief Generates a weighted discrete distribution.
   *
   * Generates a weighted discrete distribution with
   * randomly generated values within the range [1, n].
   * @param[in] n lenght of the range.
   */
  template<typename T>
  T dist(T l)
  {
    return weighted_dist_t<T>(
        l, 0, l,
        [&](double x)
        {
          return _p.coeff_const + _p.coeff_ascending * x / l +
                 _p.coeff_descending * (l - x) / l +
                 _p.coeff_sin *
                     sin(std::sqrt((x / static_cast<double>(l))) * 3.142);
        })(_gen);
  }

  /**\brief Generates a weighted discrete distribution.
   *
   * Generates a weighted discrete distribution where
   * all randomly generated values are within the
   * range [min, max] and the probability density
   * function is controlled by the parameter set _p.
   * @param[in] min lower bound of the range
   * @param[in] max upper bound of the range
   */
  size_t desc_dist(size_t min, size_t max);

  /**\brief Generates the mirrored weighted discrete distribution.
   *
   * Generates the mirrored weighted discrete distribution,
   * where the probability density function is flipped
   * between min and max.
   * All randomly generated values are within the range [min, max].
   * @param[in] min lower bound of the range
   * @param[in] max upper bound of the range
   */
  size_t asc_dist(size_t min, size_t max);

 public:
  GraphGenerator(const GraphGeneratorProperties& p)
      : _p(p), _gen(_rd()), _gen2(_rd())
  {}

  /**\brief Generates the structure of a DAG,
   * satisfying all constraints imposed by _p.
   */
  void generate_DAG(Graph& g);

  /**\brief Annotates a DAG with
   * vertex sizes,
   * model costs,
   * etc...
   */
  void annotate_DAG(Graph& g);

  /**\brief Delegates to generate_DAG and annotate_DAG.
   */
  void operator()(Graph& g);

 private:
  const GraphGeneratorProperties& _p;
  std::random_device _rd;
  std::mt19937 _gen;
  std::minstd_rand _gen2;

  /// Helper for storing desired in-edge counts.
  std::vector<size_t> _desired_in;

  /// Helper for storing desired out-edge counts.
  std::vector<size_t> _desired_out;
};

}  // end namespace admission

#endif  // GENERATOR_HPP
