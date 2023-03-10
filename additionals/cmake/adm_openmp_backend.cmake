# **************************************************************************** #
# This file is part of the ADMission build system. It searches for an OpenMP
# runtime. We use the standard CMake OpenMP module but the user can also define
# a custom OpenMP runtime library and include directory if necessary.
# **************************************************************************** #

cmake_minimum_required(VERSION 3.13)

# Custom OpenMP runtime library and include directory
set(ADM_OPENMP_RUNTIME "" CACHE PATH "Custom OpenMP runtime library.")
mark_as_advanced(ADM_OPENMP_RUNTIME)
set(ADM_OPENMP_INCLUDE_DIR "" CACHE PATH "Custom OpenMP include directory.")
mark_as_advanced(ADM_OPENMP_INCLUDE_DIR)

# Check if the include directory exists if it is defined
if(ADM_OPENMP_INCLUDE_DIR)
  if(NOT EXISTS ${ADM_OPENMP_INCLUDE_DIR})
    nag_print_error(
      "Custom OpenMP include directory '${ADM_OPENMP_INCLUDE_DIR}' not found")
  endif()

  set(CMAKE_INCLUDE_PATH ${ADM_OPENMP_INCLUDE_DIR} ${CMAKE_INCLUDE_PATH})
endif()

# Check if the OpenMP runtime library exists if it is defined
if(ADM_OPENMP_RUNTIME)
  if(NOT EXISTS ${ADM_OPENMP_RUNTIME})
    nag_print_error(
      "Custom OpenMP runtime library '${ADM_OPENMP_RUNTIME}' not found")
  endif()

  get_filename_component(_omp_lib_dir ${ADM_OPENMP_RUNTIME} DIRECTORY)
  get_filename_component(_omp_lib ${ADM_OPENMP_RUNTIME} NAME_WE)
  string(REGEX REPLACE "^lib" "" _omp_lib ${_omp_lib})

  # AppleClang doesn't come with an OpenMP header so use the custom one
  if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(CMAKE_LIBRARY_PATH ${_omp_lib_dir} ${CMAKE_LIBRARY_PATH})
  endif()
endif()

# Find OpenMP
if(ADM_USE_OPENMP)
  find_package(OpenMP COMPONENTS CXX REQUIRED)
endif()

# Override OpenMP_CXX_LIBRARIES and set OpenMP_CXX_LIBRARY_DIR if necessary
if(ADM_OPENMP_RUNTIME)
  set(OpenMP_CXX_LIBRARIES ${_omp_lib})
  set(OpenMP_CXX_LIBRARY_DIR ${_omp_lib_dir})
endif()

# Separate multiple flags
string(REPLACE " " ";" OpenMP_CXX_FLAGS "${OpenMP_CXX_FLAGS}")

# **************************************************************************** #
# Cleanup
# **************************************************************************** #
if(ADM_OPENMP_RUNTIME)
  unset(_omp_lib_dir)
  unset(_omp_lib)
endif()

# **************************************************************************** #
# Helper function to apply the OpenMP flags to a target
# **************************************************************************** #
function(adm_compile_with_openmp visibility targets)
  set(targets ${targets} ${ARGN})

  # Check visibility
  if(NOT ${visibility} STREQUAL "PRIVATE" AND
     NOT ${visibility} STREQUAL "PUBLIC" AND
     NOT ${visibility} STREQUAL "INTERFACE" AND
     NOT ${visibility} STREQUAL "NONE")
    nag_print_error(
      "First argument must be either "
      "'PRIVATE', 'PUBLIC', 'INTERFACE' or 'NONE'.")
  endif()

  if(${visibility} STREQUAL "NONE")
    set(visibility "")
  endif()

  foreach(tgt ${targets})
    if(NOT TARGET ${tgt})
      nag_print_error("OpenMP compile flags: Target ${tgt} doesn't exist.")
    endif()

    if(ADM_USE_OPENMP)
      target_compile_options(${tgt} ${visibility} ${OpenMP_CXX_FLAGS})
      if(OpenMP_CXX_INCLUDE_DIR)
        target_include_directories(
          ${tgt} SYSTEM ${visibility} ${OpenMP_CXX_INCLUDE_DIR})
      endif()
    else()
      target_compile_options(${tgt} ${visibility} "-Wno-unknown-pragmas")
    endif()
  endforeach()
endfunction()

# **************************************************************************** #
# Helper function to link the OpenMP runtime against a target
# **************************************************************************** #
function(adm_link_openmp_runtime visibility targets)
  set(targets ${targets} ${ARGN})

  # Check visibility
  if(NOT ${visibility} STREQUAL "PRIVATE" AND
     NOT ${visibility} STREQUAL "PUBLIC" AND
     NOT ${visibility} STREQUAL "INTERFACE" AND
     NOT ${visibility} STREQUAL "NONE")
    nag_print_error(
      "First argument must be either "
      "'PRIVATE', 'PUBLIC', 'INTERFACE' or 'NONE'.")
  endif()

  if(${visibility} STREQUAL "NONE")
    set(visibility "")
  endif()

  foreach(tgt ${targets})
    if(NOT TARGET ${tgt})
      nag_print_error("OpenMP runtime linking: Target ${tgt} doesn't exist.")
    endif()

    if(ADM_USE_OPENMP)
      target_link_libraries(${tgt} ${visibility} ${OpenMP_CXX_LIBRARIES})
      if(OpenMP_CXX_LIBRARY_DIR)
        target_link_directories(${tgt} PRIVATE ${OpenMP_CXX_LIBRARY_DIR})
      endif()
    endif()
  endforeach()
endfunction()
