#ifndef ADM_INC_GRAPH_IMPL_TIKZ_HPP_
#define ADM_INC_GRAPH_IMPL_TIKZ_HPP_

// ################################ INCLUDES ################################ //

#include "graph/tikz.hpp"  // IWYU pragma: keep

// ###################### TEMPLATE AND INLINE CONTENTS ###################### //

namespace admission
{

/******************************************************************************
 * @brief Wrapper for writing DAGs and face DAGs to a file specified by a
 *        path instead of an ostream.
 *
 * @tparam Graph Type of the graph to write.
 * @param[in] p path object (constructible from std::string).
 * @param[in] g Const reference to the DAG or face DAG.
 ******************************************************************************/
template<typename Graph>
ADM_ALWAYS_INLINE void write_tikz(fs::path p, const Graph& g)
{
  std::ofstream o(p);
  write_tikz(o, g);
  o.close();
}

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_GRAPH_IMPL_TIKZ_HPP_
