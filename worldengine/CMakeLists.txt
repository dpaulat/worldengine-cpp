cmake_minimum_required(VERSION 3.19)
project(libworldengine CXX)

include(cmake/protoc-config)

add_library(libworldengine STATIC source/basic.cpp
                                  source/basic.h
                                  source/biome.cpp
                                  source/biome.h
                                  source/common.cpp
                                  source/common.h
                                  source/draw.cpp
                                  source/draw.h
                                  source/generation.cpp
                                  source/generation.h
                                  source/plates.cpp
                                  source/plates.h
                                  source/world.cpp
                                  source/world.h
                                  source/simulations/biome_simulation.cpp
                                  source/simulations/biome_simulation.h
                                  World.proto)

protobuf_generate(TARGET libworldengine)

find_package(Boost)
find_package(PNG)
find_package(protobuf)
find_package(ZLIB)

target_include_directories(libworldengine PRIVATE ${PLATE_TECTONICS_INCLUDE_DIR}
                                                  ${OPENSIMPLEX_NOISE_INCLUDE_DIR}
                                                  ${Boost_INCLUDE_DIR}
                                                  ${PNG_INCLUDE_DIR}
                                                  ${protobuf_INCLUDE_DIR}
                                                  ${ZLIB_INCLUDE_DIR}
                                                  ${CMAKE_CURRENT_BINARY_DIR})
