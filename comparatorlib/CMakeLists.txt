cmake_minimum_required(VERSION 2.6)

project(comparatorlib)

include_directories(lib/include)

add_subdirectory(lib)

option (WITH_TESTS "build tests" ON)

if (WITH_TESTS)
  add_subdirectory(test)
endif()