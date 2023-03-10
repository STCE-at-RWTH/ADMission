# **************************************************************************** #
# This file is part of the ADMission build system. It generates the ADMission
# config header.
# **************************************************************************** #

# Set graph and pattern paths
set(ADM_GRAPH_READ_PATH ".")
set(ADM_GRAPH_WRITE_PATH ".")

# Generate ADMission config header
configure_file(
  "${ADM_TEMPLATE_DIR}/admission_config.hpp.in" admission_config.hpp)
