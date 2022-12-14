# **************************************************************************** #
# This file is part of the ADMisson build system. It builds the ADMission
# operations library.
# **************************************************************************** #

# Collect library sources
set(_adm_operations_sources
  accumulate.cpp
  find_eliminations.cpp
  op_sequence.cpp)

# **************************************************************************** #
# Build operations source files
# **************************************************************************** #
add_library(adm_operations_objects
  OBJECT EXCLUDE_FROM_ALL ${_adm_operations_sources})
target_include_directories(adm_operations_objects PRIVATE ${adm_include_dirs})
target_link_libraries(adm_operations_objects PRIVATE Boost::graph drag::drag)
adm_compile_with_openmp(PRIVATE adm_operations_objects)

# **************************************************************************** #
# Cleanup
# **************************************************************************** #
unset(_adm_operations_sources)
