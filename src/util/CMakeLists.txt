add_library(gittyupUtil Path.cpp)

target_link_libraries(gittyupUtil Qt5::Core)

set_target_properties(gittyupUtil PROPERTIES AUTOMOC ON)
