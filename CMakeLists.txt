cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(CMAKE_BUILD_TYPE "Debug")

project(NES VERSION 0.0.1)

file(GLOB_RECURSE NES_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE NES_HDR "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
add_executable(NES main.cpp ${NES_SRC} ${NES_HDR})
target_include_directories(NES PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

add_custom_target(
  test
  DEPENDS NES
  COMMAND ./NES < input.txt)
