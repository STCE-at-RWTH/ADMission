# **************************************************************************** #
# This file is part of the ADMisson build system. It builds the ADMission
# lower bounds library.
# **************************************************************************** #

# Collect library sources
set(_adm_lower_bounds_sources simple_min_acc_cost_bound.cpp)

# **************************************************************************** #
# Build lower bounds source files
# **************************************************************************** #
add_library(adm_lower_bounds_objects
  OBJECT EXCLUDE_FROM_ALL ${_adm_lower_bounds_sources})
target_include_directories(adm_lower_bounds_objects PRIVATE ${adm_include_dirs})
target_link_libraries(adm_lower_bounds_objects PRIVATE Boost::graph drag::drag)
adm_compile_with_openmp(PRIVATE adm_lower_bounds_objects)

# **************************************************************************** #
# Cleanup
# **************************************************************************** #
unset(_adm_lower_bounds_sources)
