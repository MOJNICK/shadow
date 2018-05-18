cmake_minimum_required(VERSION 3.0)
project(comparator)

#string( REPLACE "-O3" "-O0" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
set(CMAKE_CXX_FLAGS_RELEASE "-v -save-temps ${CMAKE_CXX_FLAGS_RELEASE}")

add_subdirectory(lib)