cmake_minimum_required(VERSION 3.19)
project(worldengine-cli CXX)

add_executable(worldengine-cli source/main.cpp
                               source/types.cpp
                               source/types.h
                               source/worldengine.cpp
                               source/worldengine.h)

target_link_libraries(worldengine-cli worldengine OpenSimplexNoise PlateTectonics)

find_package(Boost COMPONENTS log
                              program_options REQUIRED)
find_package(HDF5)
find_package(PNG)
find_package(Protobuf)

target_link_libraries(worldengine-cli Boost::log
                                      Boost::program_options
                                      HDF5::HDF5
                                      PNG::PNG
                                      protobuf::protobuf)

set_target_properties(worldengine-cli PROPERTIES OUTPUT_NAME worldengine)
