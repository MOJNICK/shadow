file(GLOB comparatortest_source_files *.cpp)
use_cxx_std()


if (WITH_TESTS)
	if(TEST_PRIVATE_PART)
		add_definitions(-DTEST_PRIVATE_PART)
	endif()
  	add_definitions(-DWITH_TESTS)
endif()

add_executable(comparatortest ${comparatortest_source_files})

target_link_libraries(comparatortest comparator)

find_package( OpenCV REQUIRED ts)
target_link_libraries( comparatortest ${OpenCV_LIBS} )
