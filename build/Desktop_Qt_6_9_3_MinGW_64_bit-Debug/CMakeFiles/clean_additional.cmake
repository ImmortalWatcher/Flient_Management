# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Flight_managerment_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Flight_managerment_autogen.dir\\ParseCache.txt"
  "Flight_managerment_autogen"
  )
endif()
