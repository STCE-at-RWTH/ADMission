# **************************************************************************** #
# This file is part of the ADMisson build system. It builds the ADMission
# optimizers library.
# **************************************************************************** #

# Collect library sources
set(_adm_optimizers_sources
  branch_and_bound.cpp
  greedy_optimizer.cpp
  min_edge_fill_in_optimizer.cpp
  min_fill_in_optimizer.cpp
  optimizer.cpp)

# **************************************************************************** #
# Build optimizers source files
# **************************************************************************** #
add_library(adm_optimizers_objects
  OBJECT EXCLUDE_FROM_ALL ${_adm_optimizers_sources})
target_include_directories(adm_optimizers_objects PRIVATE ${adm_include_dirs})
target_link_libraries(adm_optimizers_objects PRIVATE Boost::graph drag::drag)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_link_libraries(adm_optimizers_objects PRIVATE stdc++fs)
endif()
adm_compile_with_openmp(PRIVATE adm_optimizers_objects)

# **************************************************************************** #
# Cleanup
# **************************************************************************** #
unset(_adm_optimizers_sources)
