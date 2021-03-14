cmake_minimum_required(VERSION 3.19)
project(worldengine-cli CXX)

add_executable(worldengine source/main.cpp
                           source/types.cpp
                           source/types.h
                           source/worldengine.cpp
                           source/worldengine.h)

target_include_directories(worldengine PRIVATE ${libworldengine_SOURCE_DIR}/source)
target_link_libraries(worldengine libworldengine OpenSimplexNoise PlateTectonics)

find_package(Boost COMPONENTS log
                              program_options REQUIRED)
find_package(PNG)
find_package(Protobuf)

target_link_libraries(worldengine Boost::log
                                  Boost::program_options
                                  PNG::PNG
                                  protobuf::protobuf)
