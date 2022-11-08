#ifndef ACCUMULATE_HPP
#define ACCUMULATE_HPP

// ******************************** Includes ******************************** //

#include "graph/DAG.hpp"

#include <memory>

// **************************** Header contents ***************************** //

namespace admission
{

/**\brief Accumulates the Jacobian of a DAG g by simply multiplying along
 * all paths between all minimal \f$x \in X\f$ and all maximal \f$y \in Y\f$
 * vertices and adding the Jacobians \f$F'_{x,y}\f$ of all paths.
 * Constructs a tripartite face DAG \f$\tilde G'\f$ where each intermediate vertex (x,y)
 * holds the accumulated Jacobian \f$F'_{x,y}\f$.
 * g' is constructed by:
 *     For each minimal vertex x \in X
 *      Add a vertex \f$x' \in \tilde X' to \tilde g'\f$
 *     For each maximal vertex \f$y \in Y\f$
 *      Add a vertex \f$y' \in \tilde Y' to \tilde g'\f$
 *     For each edge \f$(x,y) \in E\f$
 *      Add a vertex \f$(x,y) \in \tilde V\f$
 *      Add an edge \f$(x', (x,y)) \in \tilde E\f$
 *      Add an edge \f$((x,y), y') \in \tilde E\f$
 * and the Jacobains on \f$(x,y) \in \tilde E\f$ are computed by:
 *   For each minimal vertex \f$x \in X\f$
 *     For each maximal vertex \f$y \in Y\f$
 *       For each path \f$(x,...,y)\f$
 *         Compute \f$F'_{x,...,y}\f$ the product of all elemantal Jacobians along \f$(x,...,y)\f$
 *       Compute \f$F'_{x,y} the sum of all \f$F'_{x,...,y}\f$
 *       and attach it to the corresponding vertex \f$(x,y) \in \tilde V\f$.
 *
 * @param[in] g reference to the original DAG.
 * @returns a shared_ptr to a tripartite face DAG derived from g.
 */
std::shared_ptr<FaceDAG> accumulate_trivial(const DAG& g);

}  // end namespace admission

#endif  // ACCUMULATE_HPP
