cmake_minimum_required(VERSION 2.6)
project(accordauthserver)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES 
        src/main.cpp
        src/Server.cpp
        src/log/Logger.cpp
        src/thread/Thread.cpp
        src/util/LibEventUtil.cpp
    )
find_package(Threads REQUIRED)
find_package(LibEvent REQUIRED)

add_executable(accordauthserver ${SOURCES})

target_include_directories(accordauthserver 
    PUBLIC include/)
target_include_directories(accordauthserver PUBLIC ${LIBEVENT_INCLUDE_DIRS})
target_link_libraries (accordauthserver ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (accordauthserver ${LIBEVENT_LIBRARIES})

install(TARGETS accordauthserver RUNTIME DESTINATION bin)
