include_directories(include)

file(GLOB_RECURSE comparator_source src/*)
add_library(comparator SHARED ${comparator_source})