cmake_minimum_required(VERSION 3.19)
project(libworldengine CXX)

add_library(libworldengine SHARED source/worldengine.cpp)

find_package(Boost)

if(TARGET Boost::Boost)
    include_directories(${Boost_INCLUDE_DIRS})
endif()