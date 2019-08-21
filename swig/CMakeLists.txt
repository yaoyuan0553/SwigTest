cmake_minimum_required(VERSION 3.14)
project(SwigTest)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SWIG_I_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(python)
add_subdirectory(java)
