cmake_minimum_required(VERSION 3.19)
project(libworldengine CXX)

find_package(Boost)
find_package(PNG)
find_package(Protobuf)
find_package(ZLIB)

set(SRC_MAIN source/basic.cpp
             source/biome.cpp
             source/common.cpp
             source/draw.cpp
             source/generation.cpp
             source/plates.cpp
             source/world.cpp)
set(HDR_MAIN source/basic.h
             source/biome.h
             source/common.h
             source/draw.h
             source/generation.h
             source/plates.h
             source/world.h)
set(SRC_SIMULATIONS source/simulations/biome_simulation.cpp
                    source/simulations/precipitation.cpp
                    source/simulations/temperature.cpp)
set(HDR_SIMULATIONS source/simulations/biome_simulation.h
                    source/simulations/precipitation.h
                    source/simulations/temperature.h)
set(PROTO_FILES World.proto)

add_library(worldengine STATIC ${SRC_MAIN}
                               ${HDR_MAIN}
                               ${SRC_SIMULATIONS}
                               ${HDR_SIMULATIONS}
                               ${PROTO_FILES})

protobuf_generate(TARGET worldengine)

file(GLOB HDR_AUTOGEN "${libworldengine_BINARY_DIR}/*.h")
file(GLOB SRC_AUTOGEN "${libworldengine_BINARY_DIR}/*.cc")

source_group("Protobuf Files"            FILES ${PROTO_FILES})
source_group("Header Files\\autogen"     FILES ${HDR_AUTOGEN})
source_group("Source Files\\autogen"     FILES ${SRC_AUTOGEN})
source_group("Header Files\\main"        FILES ${HDR_MAIN})
source_group("Source Files\\main"        FILES ${SRC_MAIN})
source_group("Header Files\\simulations" FILES ${HDR_SIMULATIONS})
source_group("Source Files\\simulations" FILES ${SRC_SIMULATIONS})

target_include_directories(worldengine PRIVATE ${PLATE_TECTONICS_INCLUDE_DIR}
                                               ${OPENSIMPLEX_NOISE_INCLUDE_DIR}
                                               ${Boost_INCLUDE_DIR}
                                               ${PNG_INCLUDE_DIR}
                                               ${protobuf_INCLUDE_DIR}
                                               ${ZLIB_INCLUDE_DIR}
                                               ${libworldengine_BINARY_DIR})

target_include_directories(worldengine INTERFACE ${libworldengine_SOURCE_DIR}/source)

set_target_properties(worldengine PROPERTIES CXX_STANDARD 17
                                             CXX_STANDARD_REQUIRED ON
                                             CXX_EXTENSIONS OFF)

add_subdirectory(test)
