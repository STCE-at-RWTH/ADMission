# **************************************************************************** #
# This file is part of the ADMission build system. It provides the main CMake
# entry point, which generates all necessary sources and builds the ADMission
# libraries and executables.
# **************************************************************************** #

cmake_minimum_required(VERSION 3.10.0)
cmake_policy(VERSION 3.10)

if(WIN32)
  # Enable CMAKE_MSVC_RUNTIME_LIBRARY flag
  cmake_policy(SET CMP0091 NEW)

  # No default warning flags with MSVC
  cmake_policy(SET CMP0092 NEW)
endif()

# Enable IN_LIST if() operator
cmake_policy(SET CMP0057 NEW)

# **************************************************************************** #
# Project information
# **************************************************************************** #

# Project description
set(ADM_PROJECT_DESCRIPTION "Algorithmic Differentiation Mission (ADMission)")

# Project version
set(ADM_VER_MAJOR 0)
set(ADM_VER_MINOR 1)
set(ADM_VER_PATCH 0)
set(ADM_VERSION "${ADM_VER_MAJOR}.${ADM_VER_MINOR}.${ADM_VER_PATCH}")

project(ADMission
  LANGUAGES C CXX
  DESCRIPTION ${ADM_PROJECT_DESCRIPTION}
  VERSION ${ADM_VERSION})

# **************************************************************************** #
# General setup
# **************************************************************************** #
# Require at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library / Archive output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin/")

# ADM folders & files
set(ADM_TEMPLATE_DIR "${PROJECT_SOURCE_DIR}/additionals/templates")
set(ADM_CMAKE_DIR "${PROJECT_SOURCE_DIR}/additionals/cmake")

# Include CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${ADM_CMAKE_DIR}")

# **************************************************************************** #
# CMake options
# **************************************************************************** #
option(ADM_STATIC_LIBS
  "Whether to create a static ADM library." ON)
option(ADM_SHARED_LIBS
  "Whether to create a shared ADM library." ON)
option(ADM_USE_OPENMP
  "Whether to use OpenMP." ON)
option(ADM_EXAMPLES_TARGETS
  "Whether to create targets to run the examples." ON)

# **************************************************************************** #
# Find necessary packages
# **************************************************************************** #
# Find Boost
find_package(Boost REQUIRED graph)

# Find drag (https://github.com/bigno78/drag)
add_subdirectory(additionals/3rd-party/drag-master)

# Create imported target drag::drag
add_library(drag::drag INTERFACE IMPORTED)
get_target_property(_drag_includes drag INTERFACE_INCLUDE_DIRECTORIES)
set_target_properties(drag::drag PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${_drag_includes}"
  INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${_drag_includes}")

# Include ADM modules
include(adm_openmp_backend)

# **************************************************************************** #
# Global flags
# **************************************************************************** #
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build.")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# **************************************************************************** #
# ADM include directories
# **************************************************************************** #
set(adm_include_dirs
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_BINARY_DIR}/include")

add_subdirectory(include)

# **************************************************************************** #
# Compile sources and collect libraries needed for the apps
# **************************************************************************** #
set(adm_libs)
add_subdirectory(src)
add_subdirectory(app)
add_subdirectory(examples)
