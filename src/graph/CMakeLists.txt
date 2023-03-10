# **************************************************************************** #
# This file is part of the ADMisson build system. It builds the ADMission
# graph library.
# **************************************************************************** #

# Collect library sources
set(_adm_graph_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/chain.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/face_dag.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/generator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/join.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/read_graph.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tikz.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/verify.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/write_graph.cpp)

# **************************************************************************** #
# Build graph source files
# **************************************************************************** #
add_library(adm_graph_objects OBJECT EXCLUDE_FROM_ALL ${_adm_graph_sources})
target_include_directories(adm_graph_objects PRIVATE ${adm_include_dirs})
target_link_libraries(adm_graph_objects
  PRIVATE Boost::graph drag::drag)
adm_compile_with_openmp(PRIVATE adm_graph_objects)

# **************************************************************************** #
# Cleanup
# **************************************************************************** #
unset(_adm_graph_sources)
