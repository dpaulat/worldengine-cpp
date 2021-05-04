cmake_minimum_required(VERSION 3.19)
project(libworldengine CXX)

find_package(Boost)
find_package(Eigen3)
find_package(GDAL)
find_package(HDF5)
find_package(PNG)
find_package(Protobuf)
find_package(ZLIB)

set(HDR_INTERFACE include/worldengine/common.h
                  include/worldengine/export.h
                  include/worldengine/generation.h
                  include/worldengine/plates.h
                  include/worldengine/world.h)
set(SRC_MAIN source/basic.cpp
             source/common.cpp
             source/export.cpp
             source/generation.cpp
             source/path.cpp
             source/plates.cpp
             source/world.cpp)
set(HDR_MAIN source/basic.h
             source/path.h)
set(SRC_SIMULATIONS source/simulations/biome.cpp
                    source/simulations/erosion.cpp
                    source/simulations/humidity.cpp
                    source/simulations/hydrology.cpp
                    source/simulations/icecap.cpp
                    source/simulations/irrigation.cpp
                    source/simulations/permeability.cpp
                    source/simulations/precipitation.cpp
                    source/simulations/temperature.cpp)
set(HDR_SIMULATIONS source/simulations/biome.h
                    source/simulations/erosion.h
                    source/simulations/humidity.h
                    source/simulations/hydrology.h
                    source/simulations/icecap.h
                    source/simulations/irrigation.h
                    source/simulations/permeability.h
                    source/simulations/precipitation.h
                    source/simulations/temperature.h)
set(SRC_IMAGES source/images/image.cpp
               source/images/ancient_map_image.cpp
               source/images/biome_image.cpp
               source/images/heightmap_image.cpp
               source/images/icecap_image.cpp
               source/images/ocean_image.cpp
               source/images/precipitation_image.cpp
               source/images/river_image.cpp
               source/images/satellite_image.cpp
               source/images/scatter_plot_image.cpp
               source/images/simple_elevation_image.cpp
               source/images/temperature_image.cpp)
set(HDR_IMAGES include/worldengine/images/image.h
               include/worldengine/images/ancient_map_image.h
               include/worldengine/images/biome_image.h
               include/worldengine/images/heightmap_image.h
               include/worldengine/images/icecap_image.h
               include/worldengine/images/ocean_image.h
               include/worldengine/images/precipitation_image.h
               include/worldengine/images/river_image.h
               include/worldengine/images/satellite_image.h
               include/worldengine/images/scatter_plot_image.h
               include/worldengine/images/simple_elevation_image.h
               include/worldengine/images/temperature_image.h)
set(PROTO_FILES World.proto)

add_library(worldengine STATIC ${SRC_MAIN}
                               ${HDR_MAIN}
                               ${SRC_IMAGES}
                               ${HDR_IMAGES}
                               ${SRC_SIMULATIONS}
                               ${HDR_SIMULATIONS}
                               ${HDR_INTERFACE}
                               ${PROTO_FILES})

protobuf_generate(TARGET worldengine)

file(GLOB HDR_AUTOGEN "${libworldengine_BINARY_DIR}/*.h")
file(GLOB SRC_AUTOGEN "${libworldengine_BINARY_DIR}/*.cc")

source_group("Protobuf Files"            FILES ${PROTO_FILES})
source_group("Header Files\\autogen"     FILES ${HDR_AUTOGEN})
source_group("Source Files\\autogen"     FILES ${SRC_AUTOGEN})
source_group("Header Files\\images"      FILES ${HDR_IMAGES})
source_group("Source Files\\images"      FILES ${SRC_IMAGES})
source_group("Header Files\\main"        FILES ${HDR_MAIN})
source_group("Source Files\\main"        FILES ${SRC_MAIN})
source_group("Header Files\\simulations" FILES ${HDR_SIMULATIONS})
source_group("Source Files\\simulations" FILES ${SRC_SIMULATIONS})
source_group("Header Files\\worldengine" FILES ${HDR_INTERFACE})

target_include_directories(worldengine PRIVATE ${PLATE_TECTONICS_INCLUDE_DIR}
                                               ${OPENSIMPLEX_NOISE_INCLUDE_DIR}
                                               ${Boost_INCLUDE_DIR}
                                               ${Eigen3_INCLUDE_DIR}
                                               ${GDAL_INCLUDE_DIR}
                                               ${HDF5_INCLUDE_DIRS}
                                               ${PNG_INCLUDE_DIR}
                                               ${protobuf_INCLUDE_DIR}
                                               ${ZLIB_INCLUDE_DIR}
                                               ${libworldengine_BINARY_DIR}
                                               ${libworldengine_SOURCE_DIR}/include)

# Disable LGPL features in Eigen
target_compile_definitions(worldengine PRIVATE EIGEN_MPL2_ONLY)

target_include_directories(worldengine INTERFACE ${libworldengine_SOURCE_DIR}/include)

set_target_properties(worldengine PROPERTIES CXX_STANDARD 17
                                             CXX_STANDARD_REQUIRED ON
                                             CXX_EXTENSIONS OFF)

add_subdirectory(test)
