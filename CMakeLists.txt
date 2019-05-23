# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project ("file_mover")

# Include sub-projects.
add_executable (file_mover src/file_mover.cpp src/logger.cpp src/directory_copy.cpp inc/file_mover.rc)

include_directories(inc)
install(TARGETS file_mover DESTINATION bin)