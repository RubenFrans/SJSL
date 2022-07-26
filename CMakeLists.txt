# CMake build file of SJSL
cmake_minimum_required(VERSION 3.20)

project(SJSL)

# add SJSL_Lib directory
add_subdirectory(SJSL)

# add unit tests directory
add_subdirectory(UnitTests)

