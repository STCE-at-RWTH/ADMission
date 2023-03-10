#ifndef ADM_INC_GRAPH_GENERATOR_HPP_
#define ADM_INC_GRAPH_GENERATOR_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "util/properties.hpp"

#include <boost/graph/adjacency_list.hpp>  // IWYU pragma: keep

#include <algorithm>  // IWYU pragma: keep
#include <random>
#include <stddef.h>
#include <vector>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

/******************************************************************************
 * @brief Stores all relevant properties for generating a "random" DAG.
 *
 * @note See calls to the register_property(...)
 *       functions for explanation of the parameters.
 ******************************************************************************/
class GraphGeneratorProperties : public Properties
{
 public:
  //! Constructor of GraphGeneratorProperties.
  GraphGeneratorProperties();

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

/******************************************************************************
 * @brief Generates a pseudo-random DAG.
 *
 * Generates a pseudo-random DAG ensuring certain properties
 * like number of vertices and connectivity.
 *
 * @tparam Graph Type of the graph.
 ******************************************************************************/
template<typename Graph>
class GraphGenerator
{
  //! A uniform distribution for predecessor- and successor numbers.
  template<typename T = size_t>
  using dist_t = std::uniform_int_distribution<T>;

  //! A weighted distribuiton for edge generation.
  template<typename T = size_t>
  using weighted_dist_t = std::discrete_distribution<T>;

 protected:
  //! Generates a weighted discrete distribution.
  template<typename T>
  T dist(T l);

  //! Generates a weighted discrete distribution.
  size_t desc_dist(size_t min, size_t max);

  //! Generates the mirrored weighted discrete distribution.
  size_t asc_dist(size_t min, size_t max);

 public:
  //! Constructor initializing the properties and the random devices.
  GraphGenerator(const GraphGeneratorProperties& p);

  //! Generates the structure of a DAG, satisfying all constraints
  //! imposed by _p.
  void generate_DAG(Graph& g);

  //! Annotates a DAG with vertex sizes, model costs, etc.
  void annotate_DAG(Graph& g);

  //! Delegates to generate_DAG and annotate_DAG.
  void operator()(Graph& g);

 private:
  //! Properties for the generators.
  const GraphGeneratorProperties& _p;

  //! Randomness generator.
  std::random_device _rd;
  //! Random number generator.
  std::mt19937 _gen;
  //! Random number generator.
  std::minstd_rand _gen2;

  //! Helper for storing desired in-edge counts.
  std::vector<size_t> _desired_in;

  //! Helper for storing desired out-edge counts.
  std::vector<size_t> _desired_out;
};

}  // end namespace admission

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "graph/impl/generator.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_GRAPH_GENERATOR_HPP_
