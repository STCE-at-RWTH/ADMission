# **************************************************************************** #
# This file is part of the ADMisson build system. It builds the applications.
# **************************************************************************** #

# Collect apps
set(_apps admission)

# Iterate over tests, create executables and register CTests
foreach(app_name ${_apps})
  # Create executable
  add_executable(${app_name} ${CMAKE_CURRENT_SOURCE_DIR}/${app_name}.cpp)
  target_include_directories(${app_name} PRIVATE ${adm_include_dirs})
  target_link_libraries(${app_name} PRIVATE ${adm_libs} Boost::graph)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT APPLE)
    target_link_libraries(${app_name} PRIVATE stdc++fs)
  endif()
  adm_compile_with_openmp(PRIVATE ${app_name})
  adm_link_openmp_runtime(PRIVATE ${app_name})
endforeach()

# **************************************************************************** #
# Cleanup
# **************************************************************************** #
unset(_apps)
