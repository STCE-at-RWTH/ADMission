# **************************************************************************** #
# This file is part of the ADMission build system. It prints some configuration
# information during the CMake configuration.
# **************************************************************************** #

# Print build type again
nag_print_build_type()

# OpenMP
if(ADM_USE_OPENMP)
  if(DEFINED OpenMP_C_VERSION)
    nag_print_status("ADMission uses OpenMP version ${OpenMP_C_VERSION}.")
  else()
    nag_print_status("ADMission uses OpenMP (unkonwn version).")
  endif()
  if(ADM_OPENMP_RUNTIME)
    nag_print_status("ADMission custom OpenMP runtime: ${ADM_OPENMP_RUNTIME}")
  endif()
else()
  nag_print_warning(
    "ADMission doesn't use OpenMP! This configuration is only for debug purposes.")
endif()

# Libs
if(ADM_STATIC_LIBS AND ADM_SHARED_LIBS)
  nag_print_status("ADMission libraries: static and shared")
elseif(ADM_STATIC_LIBS)
  nag_print_status("ADMission libraries: static")
elseif(ADM_SHARED_LIBS)
  nag_print_status("ADMission libraries: shared")
else()
  nag_print_error(
    "Neither static nor shared libraries are enabled!"
    "Can't build tests or application.")
endif()

# Unit Tests
if(ADM_BUILD_TESTS)
  nag_print_status("Build unit tests: ON")
else()
  nag_print_status("Build unit tests: OFF")
endif()

# Userguide
if(DEFINED ADM_BUILD_USERGUIDE)
  if(ADM_BUILD_USERGUIDE)
    nag_print_status("Build LaTeX userguide: ON")
  else()
    nag_print_status("Build LaTeX userguide: OFF")
  endif()
endif()

# Doxygen
if(DEFINED ADM_BUILD_DOXYGEN)
  if(ADM_BUILD_DOXYGEN)
    nag_print_status("Generate doxygen documentation: ON")
  else()
    nag_print_status("Generate doxygen documentation: OFF")
  endif()
endif()
