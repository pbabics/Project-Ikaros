cmake_minimum_required (VERSION 2.6)

set (LibraryName FTP)
set (ConfigName ftp.conf)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)


set(CMAKE_CXX_FLAGS " -Wall -O2 -ldl -rdynamic -pthread -g -lmysqlclient")
set(CMAKE_BUILD_TYPE debug)

set (FTP_LINK_FLAGS  ${CMAKE_CXX_FLAGS})

add_definitions(${CMAKE_CXX_FLAGS})

add_library(${LibraryName} SHARED Base.cpp)

target_link_libraries(${LibraryName} mysqlclient)

install(TARGETS ${LibraryName} DESTINATION ${InstallDirectory})

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ConfigName}")
    if (NOT EXISTS "${InstallDirectory}${ConfigName}")
        install(FILES  ${ConfigName} DESTINATION ${InstallDirectory})
    endif (NOT EXISTS "${InstallDirectory}${ConfigName}")

endif (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ConfigName}")

message("->  FTP protocol prepared")
