cmake_minimum_required (VERSION 2.6)

set (LibraryName HTTP)
set (ConfigName http.conf)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

add_library(${LibraryName} SHARED Base.cpp)
install(TARGETS ${LibraryName} DESTINATION ${InstallDirectory})

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ConfigName}")
    if (NOT EXISTS "${InstallDirectory}${ConfigName}")
        install(FILES  ${ConfigName} DESTINATION ${InstallDirectory})
    endif (NOT EXISTS "${InstallDirectory}${ConfigName}")
endif (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ConfigName}")

message("->  HTTP protocol prepared")
