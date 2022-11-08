// ******************************** Includes ******************************** //

#include "graph/write_graph.hpp"

#include "admission_config.hpp"

#include <boost/exception/exception.hpp>
#include <boost/exception/info.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphml.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/dynamic_property_map.hpp>

#include <filesystem>

// **************************** Source contents ***************************** //

namespace admission
{

namespace detail
{

void write_graphml(std::ostream& os, admission::DAG& g)
{
  boost::dynamic_properties dp;

  dp.property("has_model", boost::get(boost::edge_has_model, g));
  dp.property("index", boost::get(boost::vertex_index, g));
  dp.property("size", boost::get(boost::vertex_size, g));

  dp.property("c_tan", boost::get(boost::edge_c_tan, g));
  dp.property("c_adj", boost::get(boost::edge_c_adj, g));
  dp.property("acc_stat", boost::get(boost::edge_acc_stat, g));

  boost::write_graphml(os, g, dp);
}

void write_graphml(std::ostream& os, admission::FaceDAG& g)
{
  boost::dynamic_properties dp;

  dp.property("edge_index", boost::get(boost::edge_index, g));
  dp.property("edge_size", boost::get(boost::edge_size, g));

  dp.property("index", boost::get(boost::vertex_index, g));
  dp.property("c_tan", boost::get(boost::vertex_c_tan, g));
  dp.property("c_adj", boost::get(boost::vertex_c_adj, g));
  dp.property("has_model", boost::get(boost::vertex_has_model, g));
  dp.property("acc_stat", boost::get(boost::vertex_acc_stat, g));

  boost::write_graphml(os, g, dp);
}

}  // end namespace detail

template<typename Graph>
void write_graphml(std::string path, Graph& g)
{
  (pattern_write_path = path).remove_filename();
  std::ofstream os(path, std::ofstream::out);
  admission::detail::write_graphml(os, g);
  os.close();
}

template void write_graphml(std::string path, admission::DAG&);
template void write_graphml(std::string path, admission::FaceDAG&);

}  // end namespace admission
