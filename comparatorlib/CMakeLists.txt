cmake_minimum_required(VERSION 3.0)

set (CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}")

project(comparatorlib)

include_directories(lib/include)

add_subdirectory(lib)

option (WITH_TESTS "build tests" ON)

if (WITH_TESTS)
  add_subdirectory(test)
endif()