# **************************************************************************** #
# This file is part of the ADMisson build system. It installs some dag files.
# **************************************************************************** #

# Collect examples
set(_adm_examples
  bat
  lion
  newton_1
  newton_2
  newton_10
  openfoam
  openfoam_1
  openfoam_2)

# Global example target
if(ADM_EXAMPLES_TARGETS)
  add_custom_target(examples)
endif()

# Iterate over examples and create run targets
foreach(example_name ${_adm_examples})
  set(example_dag ${CMAKE_CURRENT_SOURCE_DIR}/${example_name}.xml)
  set(example_config ${CMAKE_CURRENT_SOURCE_DIR}/${example_name}.in)

  # Copy examples inputs into bin folder
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy
      ${example_dag} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${example_name}.xml
    COMMAND ${CMAKE_COMMAND} -E copy
      ${example_config} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${example_name}.in)

  # Create run target
  if(ADM_EXAMPLES_TARGETS)
    add_custom_target(${example_name}
      COMMAND ${CMAKE_COMMAND} -E chdir
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} ./admission ${example_name}.in
      DEPENDS admission)
    add_dependencies(examples ${example_name})
  endif()
endforeach()

# **************************************************************************** #
# Cleanup
# **************************************************************************** #
unset(_adm_examples)
unset(example_name)
unset(example_dag)
unset(example_config)
