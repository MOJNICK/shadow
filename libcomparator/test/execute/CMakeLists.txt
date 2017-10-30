file(GLOB execute_source_files *.cpp)
use_cxx_std()

add_executable(execute ${execute_source_files})

target_link_libraries(execute comparator)

find_package( OpenCV REQUIRED )
target_link_libraries( execute ${OpenCV_LIBS} )
