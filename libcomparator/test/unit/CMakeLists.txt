file(GLOB testcomparator_source_files *.cpp)
use_cxx_std()
use_pedantic_errors()


if (WITH_TESTS)
	if(TEST_PRIVATE_PART)
		add_definitions(-DTEST_PRIVATE_PART)
	endif()
  	add_definitions(-DWITH_TESTS)
endif()

add_executable(testcomparator ${testcomparator_source_files})

target_link_libraries(testcomparator comparator)

find_package( OpenCV REQUIRED ts)
target_link_libraries( testcomparator ${OpenCV_LIBS} )
