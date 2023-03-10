# **************************************************************************** #
# This file is part of the ADMisson build system. It builds the ADMission
# misc library.
# **************************************************************************** #

# Collect library sources
set(_adm_misc_sources ${CMAKE_CURRENT_SOURCE_DIR}/newton_steps.cpp)

# **************************************************************************** #
# Build lower bounds source files
# **************************************************************************** #
add_library(adm_misc_objects
  OBJECT EXCLUDE_FROM_ALL ${_adm_misc_sources})
target_include_directories(adm_misc_objects PRIVATE ${adm_include_dirs})
target_link_libraries(adm_misc_objects PRIVATE Boost::graph)
adm_compile_with_openmp(PRIVATE adm_misc_objects)

# **************************************************************************** #
# Cleanup
# **************************************************************************** #
unset(_adm_misc_sources)
