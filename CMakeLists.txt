cmake_minimum_required(VERSION 3.14)
project(SwigTest)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)

set(SWIG_CPP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_subdirectory(swig)

add_executable(Test test.cpp Semaphore.cpp Semaphore.h Thread.h ThreadPool.h ConcurrentStaticQueue.h)

target_link_libraries(Test pthread)
