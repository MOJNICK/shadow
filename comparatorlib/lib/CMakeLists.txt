file(GLOB_RECURSE comparatorlib_source src/*)
add_library(comparatorlib SHARED ${comparatorlib_source})

find_package(OpenCV REQUIRED core)
target_link_libraries(comparatorlib ${OpenCV_LIBS})
