# **************************************************************************** #
# This file is part of the ADMission build system. It generates the ADMission
# config header.
# **************************************************************************** #

# Set graph and pattern paths
set(ADM_GRAPH_READ_PATH ${PROJECT_SOURCE_DIR}/graphs)
set(ADM_GRAPH_WRITE_PATH ${PROJECT_BINARY_DIR}/graphs)
set(ADM_PATTERN_READ_PATH ${PROJECT_SOURCE_DIR}/patterns)
set(ADM_PATTERN_WRITE_PATH ${PROJECT_BINARY_DIR}/patterns)

# Create output directories
file(MAKE_DIRECTORY ${ADM_GRAPH_WRITE_PATH})
file(MAKE_DIRECTORY ${ADM_PATTERN_WRITE_PATH})

# Generate ADMission config header
configure_file(
  "${ADM_TEMPLATE_DIR}/admission_config.hpp.in" admission_config.hpp)
