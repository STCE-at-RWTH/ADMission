// ******************************** Includes ******************************** //

#include "graph/tikz.hpp"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>

#include <array>
#include <stddef.h>
#include <vector>

// The drag interface
#include <drag/drag.hpp>

// **************************** Source contents ***************************** //

namespace admission
{

/**\brief Generates positions for verties of a (face) DAG
 * by delegating to the external draw tool.
 *
 * Using the drag::sugiyama_layout(...) function. We have to
 * copy the input g into a drag::graph before.
 * @param[in] g the (face/meta) DAG.
 * @returns a vector of 2D coordinates for all vertices.
 */
template<typename Graph>
std::vector<std::array<double, 2>> draw(const Graph& g)
{
  std::vector<drag::vertex_t> v2v(num_vertices(g));
  drag::graph h;
  BOOST_FOREACH(auto v, vertices(g))
  {
    v2v[v] = h.add_node();
  }

  BOOST_FOREACH(auto e, edges(g))
  {
    h.add_edge(v2v[source(e, g)], v2v[target(e, g)]);
  }

  drag::attributes att;
  att.node_size = 0.5;
  att.node_dist = 1.0;
  att.layer_dist = 1.5;

  drag::sugiyama_layout l(h, att);

  auto nodes = l.vertices();
  std::vector<std::array<double, 2>> res;

  for (auto& v : nodes)
  {
    res.emplace_back();
    res.back()[0] = v.pos.x;
    res.back()[1] = v.pos.y;
  }
  return res;
}

void write_tikz(std::ostream& out, const DAG& g)
{
  auto index = boost::get(boost::vertex_index, g);
  auto size = boost::get(boost::vertex_size, g);

  auto pos = draw(g);

  out << "\\begin{tikzpicture}[\n";
  out << "->,>=stealth',auto,node distance=2.8cm, semithick,\n";
  out << "dag_node_t/.style = {circle,\n";
  out << "    draw=black,\n";
  out << "    text=black,\n";
  out << "    align=center,\n";
  out << "    minimum size = 12pt,\n";
  out << "    inner sep = 0pt},\n";
  out << "size/.style = {%rectangle,\n";
  out << "    draw=none,\n";
  out << "    align=center,\n";
  out << "    minimum size = 0pt,\n";
  out << "    inner sep = 1pt},\n";
  out << "every label/.append style={\n";
  out << "    inner sep = 8pt\n";
  out << "    font=\footnotesize},\n";
  out << "]\n";

  BOOST_FOREACH(auto v, vertices(g))
  {
    out << "\\node[dag_node_t] (v_" << index[v] << ") ";
    out << "at (" << pos[v][0];
    out << "," << pos[v][1] << ") ";
    out << "{$" << index[v] << "$}";
    out << ";\n";

    out << "\\node[size,right=0pt] (c_" << index[v] << ") ";
    out << "at (v_" << index[v] << ".east) ";
    out << "{\\footnotesize$" << size[v] << "$}";
    out << ";\n";
  }

  BOOST_FOREACH(auto e, edges(g))
  {
    out << "\\draw (v_" << index[source(e, g)] << ") --";
    out << "(v_" << index[target(e, g)] << ");\n";
  }
  out << "\\end{tikzpicture}";
}

void write_tikz(std::ostream& out, const FaceDAG& g)
{
  auto index = boost::get(boost::vertex_index, g);
  auto c_tan = boost::get(boost::vertex_c_tan, g);
  auto c_adj = boost::get(boost::vertex_c_adj, g);
  auto has_model = boost::get(boost::vertex_has_model, g);
  auto acc_stat = boost::get(boost::vertex_acc_stat, g);

  // Generate the positions of all vertices from the external draw tool.
  auto pos = draw(g);

  out << "\\begin{tikzpicture}[\n";
  out << "->,>=stealth',auto,node distance=2.8cm, semithick,\n";
  out << "face_dag_model_unacc_t/.style = {circle, dotted,\n";
  out << "    draw=black,\n";
  out << "    text=black,\n";
  out << "    align=center,\n";
  out << "    minimum size = 12pt,\n";
  out << "    inner sep = 0pt},\n";

  out << "face_dag_model_acc_t/.style = {circle, dashed, \n";
  out << "    draw=black,\n";
  out << "    text=black,\n";
  out << "    align=center,\n";
  out << "    minimum size = 12pt,\n";
  out << "    inner sep = 0pt},\n";

  out << "face_dag_nomodel_acc_t/.style = {circle,\n";
  out << "    draw=black,\n";
  out << "    text=black,\n";
  out << "    align=center,\n";
  out << "    minimum size = 12pt,\n";
  out << "    inner sep = 0pt},\n";

  out << "size/.style = {%rectangle,\n";
  out << "    draw=none,\n";
  out << "    align=center,\n";
  out << "    minimum size = 0pt,\n";
  out << "    inner sep = 1pt},\n";

  out << "every label/.append style={\n";
  out << "    inner sep = 8pt\n";
  out << "    font=\footnotesize},\n";
  out << "]\n";

  BOOST_FOREACH(auto v, vertices(g))
  {
    if (true /*in_degree(v, g) || out_degree(v, g)*/)
    {
      out << "\\node[";
      if (has_model[v] && !acc_stat[v])
      {
        out << "face_dag_model_unacc_t";
      }
      else if (has_model[v] && acc_stat[v])
      {
        out << "face_dag_model_acc_t";
      }
      else
      {
        out << "face_dag_nomodel_acc_t";
      }
      out << "] (v_" << index[v];
      out << ") at (" << pos[v][0];
      out << "," << pos[v][1] << ")";

      if (in_degree(v, g) && out_degree(v, g))
      {
        auto [ij, jk] = index_pair(v, g);
        out << "{\\footnotesize$" << ij << ", " << jk << "$};\n";
      }
      else
      {
        out << "{};\n";
      }

      if (has_model[v])
      {
        out << "\\node[size,right=0pt,at=(v_" << index[v];
        out << ".east)]{\\tiny$" << c_adj[v] << "$\\\\ \\tiny$" << c_tan[v]
            << "$};\n";
      }
    }
  }

  BOOST_FOREACH(auto e, edges(g))
  {
    out << "\\draw (v_" << index[source(e, g)] << ") --";
    out << "(v_" << index[target(e, g)] << ");\n";
  }
  out << "\\end{tikzpicture}";
}

void write_tikz(std::ostream& out, const MetaDAG& g)
{
  auto info = boost::get(boost::vertex_info, g);
  size_t opt_idx = boost::get_property(g, boost::graph_opt);

  // Generate the positions of all vertices from the external draw tool.
  auto pos = draw(g);

  out << "\\begin{tikzpicture}[\n";
  out << "->,>=stealth',auto,node distance=2.8cm, semithick,\n";
  out << "cut_t/.style = {circle, dotted,\n";
  out << "    draw=black,\n";
  out << "    text=black,\n";
  out << "    align=center,\n";
  out << "    minimum size = 12pt,\n";
  out << "    inner sep = 0pt},\n";

  out << "was_opt_t/.style = {circle, dashed, \n";
  out << "    draw=black,\n";
  out << "    text=black,\n";
  out << "    align=center,\n";
  out << "    minimum size = 12pt,\n";
  out << "    inner sep = 0pt},\n";

  out << "opt_t/.style = {circle, dashed, \n";
  out << "    fill=gray,\n";
  out << "    draw=black,\n";
  out << "    text=black,\n";
  out << "    align=center,\n";
  out << "    minimum size = 12pt,\n";
  out << "    inner sep = 0pt},\n";

  out << "inter_t/.style = {circle,\n";
  out << "    draw=black,\n";
  out << "    text=black,\n";
  out << "    align=center,\n";
  out << "    minimum size = 12pt,\n";
  out << "    inner sep = 0pt},\n";

  out << "size/.style = {%rectangle,\n";
  out << "    draw=none,\n";
  out << "    align=center,\n";
  out << "    minimum size = 0pt,\n";
  out << "    inner sep = 1pt},\n";

  out << "every label/.append style={\n";
  out << "    inner sep = 8pt\n";
  out << "    font=\footnotesize},\n";
  out << "]\n";

  BOOST_FOREACH(auto v, vertices(g))
  {
    out << "\\node[";
    if (info[v].cut == true)
    {
      out << "cut_t";
    }
    else if (v == opt_idx)
    {
      out << "opt_t";
    }
    else if (info[v].was_opt == true)
    {
      out << "was_opt_t";
    }
    else
    {
      out << "inter_t";
    }
    out << "] (v_" << v;
    out << ") at (" << pos[v][0];
    out << "," << pos[v][1] << ")";

    out << "{$" << v << "$};\n";
  }

  BOOST_FOREACH(auto e, edges(g))
  {
    out << "\\draw (v_" << source(e, g) << ") --";
    out << "(v_" << target(e, g) << ");\n";
  }
  out << "\\end{tikzpicture}";
}

}  // end namespace admission
