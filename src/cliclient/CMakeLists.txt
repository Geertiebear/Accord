cmake_minimum_required(VERSION 2.6)
project (accordcliclient)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
        src/main.cpp
        src/Commands.cpp
        src/CommandParser.cpp
    )

add_executable(accordcliclient ${SOURCES})

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
endif (CMAKE_COMPILER_IS_GNUCC)
if (MSVC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif (MSVC)

target_include_directories(accordcliclient
    PUBLIC include/)

install(TARGETS accordcliclient RUNTIME DESTINATION bin)
