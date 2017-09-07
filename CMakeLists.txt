cmake_minimum_required(VERSION 2.6)
project(accordauthserver)

add_executable(accordauthserver main.cpp)

install(TARGETS accordauthserver RUNTIME DESTINATION bin)
