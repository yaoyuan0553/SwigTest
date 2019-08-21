cmake_minimum_required(VERSION 3.14)
project(TestSwigPython)

find_package(SWIG)
include(UseSWIG)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/swig_out/python)

set_property(SOURCE ${SWIG_I_SOURCE_DIR}/library.i PROPERTY CPLUSPLUS ON)

set_property(SOURCE ${SWIG_I_SOURCE_DIR}/library.i PROPERTY SWIG_MODULE_NAME TestSwigPython)

swig_add_library(
        TestSwigPython
        TYPE SHARED
        LANGUAGE python
        OUTPUT_DIR ${CMAKE_SOURCE_DIR}/swig_out/python
        OUTFILE_DIR ${CMAKE_SOURCE_DIR}/swig_out/python
        SOURCES ${SWIG_I_SOURCE_DIR}/library.i ${SWIG_CPP_SOURCE_DIR}/library.cpp
)

target_include_directories(
        TestSwigPython PRIVATE
        /usr/include/python3.6
        ${CMAKE_CURRENT_SOURCE_DIR}
)
