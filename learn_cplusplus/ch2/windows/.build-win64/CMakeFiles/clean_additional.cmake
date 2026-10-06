# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles/calculator_qml_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/calculator_qml_autogen.dir/ParseCache.txt"
  "calculator_qml_autogen"
  )
endif()
