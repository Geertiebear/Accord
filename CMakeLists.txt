cmake_minimum_required(VERSION 2.6)
project(accord)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#sources
set(SOURCES_SERVER
        src/server/main.cpp
        src/server/Server.cpp
        src/server/log/Logger.cpp
        src/server/thread/Thread.cpp
        src/server/util/LibEventUtil.cpp
        src/server/util/OpenSSLUtil.cpp
        src/server/network/PacketHandlers.cpp
    )
set(SOURCES_CLICLIENT
    src/cliclient/main.cpp
    src/cliclient/Commands.cpp
    src/cliclient/CommandParser.cpp
    )
set(SOURCES_SHARED
    src/shared/network/PacketDecoder.cpp
    src/shared/network/PacketHandler.cpp
    src/shared/network/packet/ErrorPacket.cpp
    src/shared/network/packet/SendMessagePacket.cpp
    )
#libraries
find_package(Threads REQUIRED)
find_package(LibEvent REQUIRED)
find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)

add_library(libaccordshared SHARED ${SOURCES_SHARED})

#execs
add_executable(accordserver ${SOURCES_SERVER})
add_executable(accordcliclient ${SOURCES_CLICLIENT})

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
endif (CMAKE_COMPILER_IS_GNUCC)
if (MSVC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif (MSVC)

#linking
target_include_directories(libaccordshared
    PUBLIC include/)
target_link_libraries(libaccordshared ${LIBEVENT_LIBRARIES})

target_include_directories(accordserver 
    PUBLIC include/)
target_include_directories(accordserver PUBLIC ${LIBEVENT_INCLUDE_DIRS})
target_include_directories(accordserver PUBLIC ${Boost_INCLUDE_DIR})
target_include_directories(accordserver PUBLIC ${OPENSSL_INCLUDE_DIR})
target_link_libraries(accordserver ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(accordserver ${Boost_LIBRARIES})
target_link_libraries(accordserver ${LIBEVENT_LIBRARIES})
target_link_libraries(accordserver ${OPENSSL_LIBRARIES})
target_link_libraries(accordserver libaccordshared)

target_include_directories(accordcliclient
    PUBLIC include/)
target_link_libraries(accordcliclient libaccordshared)

#install
install(TARGETS accordserver accordcliclient RUNTIME DESTINATION bin)
