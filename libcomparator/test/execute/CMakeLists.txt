file(GLOB execute_source_files *.cpp)
use_cxx_std()

include_directories(/usr/include/python2.7)

add_executable(execute ${execute_source_files})


target_link_libraries(execute comparator)


find_library(python python2.7)
if(python)
	target_link_libraries(execute python2.7)
else()
	message( FATAL_ERROR "Cannot find libpython")
endif()

find_package( OpenCV REQUIRED )
target_link_libraries( execute ${OpenCV_LIBS} )