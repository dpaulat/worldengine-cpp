cmake_minimum_required(VERSION 3.19)
project(libworldengine CXX)

include(cmake/protoc-config)

add_library(libworldengine STATIC source/biome.cpp
                                  source/biome.h
                                  source/common.cpp
                                  source/common.h
                                  source/generation.cpp
                                  source/generation.h
                                  source/plates.cpp
                                  source/plates.h
                                  source/world.cpp
                                  source/world.h
                                  World.proto)

protobuf_generate(TARGET libworldengine)

find_package(Boost)
find_package(protobuf)

target_include_directories(libworldengine PRIVATE ${WORLDENGINE_DIR}/external/plate-tectonics/src)
target_include_directories(libworldengine PRIVATE ${PYTHON_INCLUDE_DIR})
target_include_directories(libworldengine PRIVATE ${Boost_INCLUDE_DIR})
target_include_directories(libworldengine PRIVATE ${protobuf_INCLUDE_DIR})
target_include_directories(libworldengine PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
