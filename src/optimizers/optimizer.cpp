// ******************************** Includes ******************************** //

#include "optimizers/optimizer.hpp"

#include "graph/tikz.hpp"
#include "graph/utils.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <stddef.h>

// **************************** Source contents ***************************** //

namespace admission
{

bool Optimizer::check_if_solved(const FaceDAG& g) const
{
  if (2 != longest_path(g))
  {
    return false;
  }
  else
  {
    auto Fprime_exists = boost::get(boost::vertex_acc_stat, g);
    BOOST_FOREACH(auto ij, vertices(g))
    {
      if (in_degree(ij, g) && out_degree(ij, g) && !(Fprime_exists[ij]))
      {
        return false;
      }
    }
  }
  return true;
}

/**Prints the meta DAG that is build when _diagnostics is turned on
 */
void Optimizer::print_meta_DAG(std::ostream& o) const
{
  o << "\\documentclass{standalone}" << std::endl;
  o << "\\usepackage{graphicx}" << std::endl;
  o << "\\usepackage{tikz}" << std::endl;
  o << "\\usetikzlibrary";
  o << "{shapes,arrows,automata,decorations.pathreplacing,angles,quotes}";
  o << std::endl;
  o << "\\begin{document}" << std::endl;
  admission::write_tikz(o, _meta_dag);
  o << std::endl;
  for (size_t k = 0; k < num_vertices(_meta_dag); ++k)
  {
    o << "\\input{" << k << ".tex}" << std::endl;
  }
  o << "\\end{document}" << std::endl;
}

void Optimizer::print_meta_DAG(fs::path p) const
{
  std::ofstream o(p);
  print_meta_DAG(o);
  o.close();
}

}  // end namespace admission
