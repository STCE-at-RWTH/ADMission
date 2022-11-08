// ******************************** Includes ******************************** //

#include "graph/DAG.hpp"
#include "graph/read_graph.hpp"
#include "graph/tikz.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

// **************************** Source contents ***************************** //

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    return -1;
  }

  admission::DAG g;
  try
  {
    admission::read_graph(argv[1], g);
  }
  catch (std::runtime_error& e)
  {
    std::cout << e.what() << std::endl;
    return -1;
  }

  std::ofstream o(std::string(argv[1]) + ".tex");
  o << "\\documentclass{standalone}" << std::endl;
  o << "\\usepackage{graphicx}" << std::endl;
  o << "\\usepackage{tikz}" << std::endl;
  o << "\\usetikzlibrary";
  o << "{shapes,arrows,automata,decorations.pathreplacing,angles,quotes}";
  o << std::endl;
  o << "\\begin{document}" << std::endl;
  admission::write_tikz(o, g);
  o << std::endl;
  o << "\\end{document}" << std::endl;
  o.close();

  return 0;
}
