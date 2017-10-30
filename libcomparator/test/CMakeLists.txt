if (WITH_TESTS)
	if(TEST_PRIVATE_PART)
		add_definitions(-DTEST_PRIVATE_PART)
	endif()
  	add_definitions(-DWITH_TESTS)
  	add_subdirectory(unit)
  	add_subdirectory(execute)
endif()

