# **************************************************************************** #
# This file is part of the ADMission build system.
# **************************************************************************** #

# Build ADM graph objects
add_subdirectory(graph)

# Build ADM lower bounds objects
add_subdirectory(lower_bounds)

# Build ADM misc objects
add_subdirectory(misc)

# Build ADM operations objects
add_subdirectory(operations)

# Build ADM optimizers objects
add_subdirectory(optimizers)

# **************************************************************************** #
# Collect object files and create libraries
# **************************************************************************** #
set(_adm_lib_objects
  $<TARGET_OBJECTS:adm_graph_objects>
  $<TARGET_OBJECTS:adm_lower_bounds_objects>
  $<TARGET_OBJECTS:adm_misc_objects>
  $<TARGET_OBJECTS:adm_operations_objects>
  $<TARGET_OBJECTS:adm_optimizers_objects>)

# Create shared library
if(ADM_SHARED_LIBS)
  add_library(adm_shared SHARED ${_adm_lib_objects})
  set_target_properties(adm_shared PROPERTIES OUTPUT_NAME adm)
  target_link_libraries(adm_shared PRIVATE Boost::graph)
  adm_link_openmp_runtime(PRIVATE adm_shared)

  # Use shared lib for tests
  list(PREPEND adm_libs adm_shared)
endif()

# Create static library
if(ADM_STATIC_LIBS)
  add_library(adm_static STATIC ${_adm_lib_objects})
  set_target_properties(adm_static PROPERTIES OUTPUT_NAME adm)
  target_link_libraries(adm_static PRIVATE Boost::graph)
  adm_link_openmp_runtime(PRIVATE adm_static)

  # Use static lib for tests if shared libs are disabled
  if(NOT ADM_SHARED_LIBS)
    list(PREPEND adm_libs adm_static)
  endif()
endif()

# Propagate adm libs to the parent directory
set(adm_libs ${adm_libs} PARENT_SCOPE)

# **************************************************************************** #
# Cleanup
# **************************************************************************** #
unset(_adm_lib_objects)

