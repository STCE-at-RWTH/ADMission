cmake_minimum_required(VERSION 3.15)

# Quick return if drag is already imported
if(drag_FOUND)
  return()
endif()

set(DRAG_REPOSITORY "https://github.com/bigno78/drag")
set(DRAG_TAG "master" CACHE STRING "The drag tag which is fetched.")
mark_as_advanced(DRAG_TAG)

# Fetch drag
message(STATUS
  "drag: Fetching version '${DRAG_TAG}' from '${DRAG_REPOSITORY}'.")

# Necessary packages
include(FetchContent)
find_package(Git QUIET)

# Declare drag
FetchContent_Declare(
  drag
  GIT_REPOSITORY ${DRAG_REPOSITORY}
  GIT_TAG ${DRAG_TAG}
  GIT_SHALLOW ON
  SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/drag-${DRAG_TAG})

# Fetch drag
FetchContent_GetProperties(drag)
if(NOT drag_POPULATED)
  FetchContent_Populate(drag)
  add_subdirectory(${drag_SOURCE_DIR} ${drag_BINARY_DIR})
endif()

# Print sha
execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
  WORKING_DIRECTORY ${drag_SOURCE_DIR}
  OUTPUT_VARIABLE drag_sha)

string(REPLACE "\n" "" drag_sha ${drag_sha})
message(STATUS
  "drag: Successfully fetched '${DRAG_TAG}' HEAD at SHA ${drag_sha}")

# Check if fetch was succesful
if(EXISTS "${drag_SOURCE_DIR}")
  # Create imported target drag::drag
  add_library(drag::drag INTERFACE IMPORTED)

  # Set include directories
  get_target_property(_drag_includes drag INTERFACE_INCLUDE_DIRECTORIES)
  set_target_properties(drag::drag PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${_drag_includes}")
  set(drag_FOUND 1)
  unset(_drag_includes)
else()
  set(drag_FOUND 0)
  message(FATAL_ERROR "drag: Fetched version invalid.")
endif()
