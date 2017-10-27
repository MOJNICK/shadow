file(GLOB comparatortest_source_files *.cpp)
add_executable(comparatortest ${comparatortest_source_files})

target_link_libraries(comparatortest comparatorlib)

find_package( OpenCV REQUIRED ts)
target_link_libraries( comparatortest ${OpenCV_LIBS} )
