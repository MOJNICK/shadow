use_cxx_std()

file(GLOB_RECURSE comparator_source src/*)
add_library(comparator SHARED ${comparator_source})

find_package(OpenCV REQUIRED core)
target_link_libraries(comparator ${OpenCV_LIBS})
