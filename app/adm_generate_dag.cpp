// ******************************** Includes ******************************** //

#include "graph/DAG.hpp"
#include "graph/generator.hpp"
#include "graph/tikz.hpp"
#include "graph/write_graph.hpp"
#include "properties.hpp"

#include <fstream>
#include <iostream>
#include <omp.h>

// **************************** Source contents ***************************** //

/**Interface for the generation of peseudo-random DAGs.
 * Parameters are retrieved from a file using the
 * Properties class and the output is written as a
 * compilable laTeX file as well as a GRAPHML file.
 */
int main(int argc, char* argv[])
{
  admission::GraphGeneratorProperties p;

  if (argc != 2)
  {
    p.print(std::cout);
    return -1;
  }

  try
  {
    p.parse_config(std::ifstream(argv[1]));
  }
  catch (const admission::BadConfigFileError& bcfe)
  {
    std::cout << bcfe.what() << std::endl;
    return -1;
  }
  catch (const admission::KeyNotRegisteredError& knre)
  {
    std::cout << knre.what() << std::endl;
    return -1;
  }

  admission::GraphGenerator<admission::DAG> gen(p);
  admission::DAG g;

  std::cout << "Generating Graph..." << std::endl;
  auto time = omp_get_wtime();
  gen(g);
  time = omp_get_wtime() - time;
  std::cout << "Done after " << time << "s" << std::endl;

  std::ofstream o("adm_generated_dag.tex");
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

  admission::write_graphml("adm_generated_dag.xml", g);

  return 0;
}
