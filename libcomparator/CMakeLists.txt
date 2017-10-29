cmake_minimum_required(VERSION 3.0)

macro(use_cxx_std)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14") 
endmacro(use_cxx_std)

use_cxx_std()

option(CMAKE_BUILD_TYPE "" Release) 
option(TEST_PRIVATE_PART "define private public, and include private test code" OFF)

if(TEST_PRIVATE_PART)
	add_definitions(-DTEST_PRIVATE_PART)
endif()

project(comparator)

include_directories(lib/include)

add_subdirectory(lib)

option (WITH_TESTS "build tests" ON)

if (WITH_TESTS)
  add_subdirectory(test)
  add_definitions(-DWITH_TESTS)
endif()