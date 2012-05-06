cmake_minimum_required (VERSION 2.6)

set (LibraryName dump)


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

add_library(${LibraryName} SHARED libdump.cpp)
install(TARGETS ${LibraryName} DESTINATION ${InstallDirectory})

message("->  libDummp prepared")
