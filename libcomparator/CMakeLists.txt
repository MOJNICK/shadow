cmake_minimum_required(VERSION 3.0)
project(comparator)
macro(use_cxx_std)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14") 
endmacro(use_cxx_std)

use_cxx_std()
option(CMAKE_BUILD_TYPE "" Release) 

include_directories(lib/include)

option (WITH_TESTS "build tests" OFF)
option(TEST_PRIVATE_PART "define private public, and include private test code" OFF)

if (WITH_TESTS)
	if(TEST_PRIVATE_PART)
		add_definitions(-DTEST_PRIVATE_PART)
	endif()
  	add_definitions(-DWITH_TESTS)
  	add_subdirectory(test)
endif()

add_subdirectory(lib)