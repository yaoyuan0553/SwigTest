cmake_minimum_required(VERSION 3.14)
project(TestSwigJava)

find_package(SWIG)
include(UseSWIG)

set(CMAKE_CXX_STANDARD 17)

set(SWIG_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/swig_out/java)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SWIG_OUTPUT_DIR})

set_property(SOURCE ${SWIG_I_SOURCE_DIR}/library.i PROPERTY CPLUSPLUS ON)

set_property(SOURCE ${SWIG_I_SOURCE_DIR}/library.i PROPERTY SWIG_MODULE_NAME TestSwigJava)

swig_add_library(
        TestSwigJava
        TYPE SHARED
        LANGUAGE java
        OUTPUT_DIR ${SWIG_OUTPUT_DIR}
        OUTFILE_DIR ${SWIG_OUTPUT_DIR}
        SOURCES ${SWIG_I_SOURCE_DIR}/library.i ${SWIG_CPP_SOURCE_DIR}/library.cpp
)

target_include_directories(
        TestSwigJava PRIVATE
        /usr/share/jdk1.8.0_202/include
        /usr/share/jdk1.8.0_202/include/linux
        ${CMAKE_CURRENT_SOURCE_DIR}
)

#set(JAVA_OUT_PATH ${CMAKE_SOURCE_DIR}/java_out_test)
set(JAVA_OUT_PATH /home/yuan/IdeaProjects/SwigTest/src/main/java)

message(STATUS "copying output files to [${JAVA_OUT_PATH}]")
add_custom_command(TARGET TestSwigJava POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${SWIG_OUTPUT_DIR}" "${JAVA_OUT_PATH}")

