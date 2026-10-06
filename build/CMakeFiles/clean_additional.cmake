# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/Day3_Counter_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/Day3_Counter_autogen.dir/ParseCache.txt"
  "Day3_Counter_autogen"
  )
endif()
