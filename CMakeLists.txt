cmake_minimum_required(VERSION 3.0)
project(accord)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#sources
set(SOURCES_SERVER
        src/server/main.cpp
        src/server/Server.cpp
        src/server/Arguments.cpp
        src/server/Config.cpp
        src/server/Authentication.cpp
        src/server/log/Logger.cpp
        src/server/thread/Thread.cpp
        src/server/util/LibEventUtil.cpp
        src/server/util/OpenSSLUtil.cpp
        src/server/util/CryptoUtil.cpp
        src/server/network/PacketHandlers.cpp
        src/server/database/Database.cpp
    )
set(SOURCES_CLICLIENT
    src/cliclient/main.cpp
    src/cliclient/Commands.cpp
    src/cliclient/CommandParser.cpp
    )
set(SOURCES_CLIENT
    src/client/main.cpp
    src/client/backend.cpp
    src/client/util.cpp
    src/client/communityimageprovider.cpp)
set(SOURCES_SHARED
    src/shared/network/PacketDecoder.cpp
    src/shared/network/PacketHandler.cpp
    src/shared/network/packet/ErrorPacket.cpp
    src/shared/network/packet/SendMessagePacket.cpp
    src/shared/network/packet/DisconnectPacket.cpp
    src/shared/network/packet/AuthPacket.cpp
    src/shared/network/packet/RegisterPacket.cpp
    src/shared/network/packet/TokenPacket.cpp
    src/shared/network/packet/SerializationPacket.cpp
    src/shared/util/BinUtil.cpp
    )
#libraries

if (MSYS)
	set(CMAKE_PREFIX_PATH /mingw64/)
endif(MSYS)

find_package(Qt5Core CONFIG REQUIRED Qml Quick Gui QuickWidgets Network)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5QuickWidgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(MySQL REQUIRED)
find_package(Threads REQUIRED)
find_package(LibEvent REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Cereal REQUIRED)
find_package(ImageMagick COMPONENTS Magick++ REQUIRED)

pkg_search_module(argon2 libargon2)

add_library(accordshared SHARED ${SOURCES_SHARED})

#QT specifics
qt5_add_resources(qml_QRC src/client/client.qrc)

#execs
add_executable(accordserver ${SOURCES_SERVER})
add_executable(accordcliclient ${SOURCES_CLICLIENT})
add_executable(accordclient MACOSX_BUNDLE ${SOURCES_CLIENT} ${qml_QRC})

#defines
target_compile_definitions(accordserver PRIVATE MAGICKCORE_HDRI_ENABLE=0
    MAGICKCORE_QUANTUM_DEPTH=16)

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
endif (CMAKE_COMPILER_IS_GNUCC)
if (MSVC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif (MSVC)

#linking
target_include_directories(accordshared
    PUBLIC include/)
target_link_libraries(accordshared ${LIBEVENT_LIBRARIES})
target_include_directories(accordshared PUBLIC ${CEREAL_INCLUDE_DIRS})
target_include_directories(accordshared PRIVATE ${Boost_INCLUDE_DIR})

target_include_directories(accordserver 
    PUBLIC include/)
target_include_directories(accordserver PUBLIC ${LIBEVENT_INCLUDE_DIRS})
target_include_directories(accordserver PUBLIC ${Boost_INCLUDE_DIR})
target_include_directories(accordserver PUBLIC ${OPENSSL_INCLUDE_DIR})
target_include_directories(accordserver PUBLIC ${MYSQL_INCLUDE_DIRS})
target_include_directories(accordserver PUBLIC ${ImageMagick_INCLUDE_DIRS})
target_include_directories(accordserver PUBLIC ${CEREAL_INCLUDE_DIRS})

if(WIN32)
	target_link_libraries(accordserver ws2_32)
endif(WIN32)

target_link_libraries(accordserver ${MYSQL_LIBRARIES})
target_link_libraries(accordserver ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(accordserver ${Boost_LIBRARIES})
target_link_libraries(accordserver ${LIBEVENT_LIBRARIES})
target_link_libraries(accordserver ${LIBEVENT_OPENSSL})
target_link_libraries(accordserver ${OPENSSL_LIBRARIES})
target_link_libraries(accordserver ${YAML_CPP_LIBRARIES})
target_link_libraries(accordserver ${argon2_LIBRARIES} ${argon2_LDFLAGS})
target_link_libraries(accordserver ${ImageMagick_LIBRARIES})
target_link_libraries(accordserver accordshared)

target_include_directories(accordcliclient
    PUBLIC include/)
target_include_directories(accordcliclient PUBLIC ${CEREAL_INCLUDE_DIRS})
target_include_directories(accordcliclient PUBLIC ${OPENSSL_INCLUDE_DIR})
target_link_libraries(accordcliclient accordshared)
if(WIN32)
	target_link_libraries(accordcliclient ws2_32)
endif(WIN32)
target_link_libraries(accordcliclient ${OPENSSL_LIBRARIES})

target_include_directories(accordclient
    PUBLIC include/)
target_include_directories(accordclient PUBLIC ${OPENSSL_INCLUDE_DIR})
target_include_directories(accordclient PUBLIC ${CEREAL_INCLUDE_DIRS})
target_link_libraries(accordclient accordshared)
target_link_libraries(accordclient ${OPENSSL_LIBRARIES})
target_link_libraries(accordclient Qt5::Core)
target_link_libraries(accordclient Qt5::Widgets)
target_link_libraries(accordclient Qt5::Qml)
target_link_libraries(accordclient Qt5::Quick)
target_link_libraries(accordclient Qt5::QuickWidgets)
target_link_libraries(accordclient Qt5::Network)

#install
if (LINUX)
	install(TARGETS accordserver accordcliclient accordclient RUNTIME DESTINATION bin)
	install(TARGETS accordshared LIBRARY DESTINATION lib)

	#packaging
	SET(CPACK_GENERATOR "DEB")
	SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Geert Custers")
	set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

	INCLUDE(CPack)
endif()
