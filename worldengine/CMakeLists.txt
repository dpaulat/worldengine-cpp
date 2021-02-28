cmake_minimum_required(VERSION 3.19)
project(libworldengine CXX)

add_library(libworldengine STATIC source/common.cpp
                                  source/common.h
                                  source/plates.cpp
                                  source/plates.h
                                  source/world.cpp
                                  source/world.h
                                  source/worldengine.cpp
                                  source/worldengine.h)

include_directories(${WORLDENGINE_DIR}/external/plate-tectonics/src)

find_package(Boost)

if(TARGET Boost::Boost)
    target_include_directories(libworldengine ${Boost_INCLUDE_DIRS})
    target_link_libraries(libworldengine ${Boost_LIBS})
endif()