cmake_minimum_required (VERSION 2.6)

project(MultiServer)
enable_language(CXX)

set (MultiServer_VERSION_MAJOR 0)
set (MultiServer_VERSION_MINOR 7)

if (Prefix)
    set (InstallDirectory ${Prefix}/bin/)
else ()
    set (InstallDirectory ${CMAKE_BINARY_DIR}/bin/)
endif ()

find_program(_GIT_EXEC
  NAMES
    git git.cmd
  HINTS
    ENV PATH
  DOC "git installation path"
)

set(_GIT_COMMIT_DATE "0000-00-00 00:00:00 +0000")
if (_GIT_EXEC)
  execute_process(
    COMMAND "${_GIT_EXEC}" log --pretty=%H -n 1
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    OUTPUT_VARIABLE _GIT_LONG_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
  execute_process(
    COMMAND "${_GIT_EXEC}" log --pretty=%h -n 1
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    OUTPUT_VARIABLE _GIT_SHORT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
  execute_process(
    COMMAND "${_GIT_EXEC}" log --pretty=%aD -n 1
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    OUTPUT_VARIABLE _GIT_COMMIT_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
message("Commit ${_GIT_LONG_COMMIT_HASH} Short Commit ${_GIT_SHORT_COMMIT_HASH} Commit Date ${_GIT_COMMIT_DATE}")

endif(_GIT_EXEC)


configure_file(
    "${CMAKE_SOURCE_DIR}/revision.h.in.cmake"
    "${CMAKE_BINARY_DIR}/revision.h"
    @ONLY
)

include_directories(
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/Application/
    ${PROJECT_SOURCE_DIR}/Handling/
    ${PROJECT_SOURCE_DIR}/Shared/
    ${PROJECT_SOURCE_DIR}/DataArrays/
    ${PROJECT_SOURCE_DIR}/SimpleLog/
    ${PROJECT_SOURCE_DIR}/ConfigMgr/
    ${PROJECT_SOURCE_DIR}/Signal/
    ${PROJECT_SOURCE_DIR}/Threads/
    ${PROJECT_SOURCE_DIR}/Sockets/
    ${PROJECT_SOURCE_DIR}/Utilities/
    ${PROJECT_SOURCE_DIR}/Daemon/
    ${PROJECT_SOURCE_DIR}/MySQL/
    ${CMAKE_BINARY_DIR}
)

set(CMAKE_CXX_FLAGS " -W -Wall -O2 -ldl -rdynamic -pthread -g")
set(CMAKE_BUILD_TYPE debug)

set (MultiServer_LINK_FLAGS  ${CMAKE_CXX_FLAGS})

add_definitions(${CMAKE_CXX_FLAGS})


set (STAT_CORE
    Utilities/Utils.cpp
    DataArrays/binData.cpp
    DataArrays/file.cpp
    SimpleLog/SimpleLog.cpp
    Shared/Shared.cpp
    Signal/SignalHandler.cpp
    Sockets/Socket.cpp
    Sockets/SocketMgr.cpp
    Threads/ThreadMgr.cpp
    ConfigMgr/ConfigMgr.cpp
    Handling/Handler.cpp
    Application/Application.cpp
    Daemon/Daemon.cpp
)

set (STAT_MAIN main.cpp)

message("")
message("***** Preparing Core *****")

add_library(Core STATIC ${STAT_CORE})
target_link_libraries(Core ${CMAKE_DL_LIBS} Core)

add_dependencies(Core revision.h)

add_executable(MultiServer ${STAT_MAIN})

set_target_properties(MultiServer PROPERTIES LINK_FLAGS ${MultiServer_LINK_FLAGS})
target_link_libraries(MultiServer ${CMAKE_DL_LIBS} Core)

message("->  Core prepared")

if (Protocols)
    add_subdirectory(Protocol)
endif (Protocols)

file(MAKE_DIRECTORY ${InstallDirectory})

install(TARGETS MultiServer DESTINATION ${InstallDirectory})
if (NOT EXISTS "${InstallDirectory}config.conf")
    install(FILES  config.conf DESTINATION ${InstallDirectory})
endif (NOT EXISTS "${InstallDirectory}config.conf")
