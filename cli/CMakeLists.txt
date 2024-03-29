cmake_minimum_required(VERSION 3.11)
project(worldengine-cli CXX)

add_executable(worldengine-cli source/main.cpp
                               source/types.cpp
                               source/types.h
                               source/worldengine.cpp
                               source/worldengine.h)

target_link_libraries(worldengine-cli worldengine OpenSimplexNoise PlateTectonics)

find_package(Boost COMPONENTS log
                              program_options REQUIRED)
find_package(GDAL)
find_package(HDF5)
find_package(PNG)
find_package(Protobuf)

target_link_libraries(worldengine-cli Boost::log
                                      Boost::program_options
                                      GDAL::GDAL
                                      HDF5::HDF5
                                      PNG::PNG
                                      protobuf::protobuf)

target_compile_options(worldengine-cli PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
                                               $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>)

set_target_properties(worldengine-cli PROPERTIES CXX_STANDARD 17
                                                 CXX_STANDARD_REQUIRED ON
                                                 CXX_EXTENSIONS OFF)
set_target_properties(worldengine-cli PROPERTIES ARCHIVE_OUTPUT_NAME worldengine-cli)
set_target_properties(worldengine-cli PROPERTIES OUTPUT_NAME worldengine)

if (MSVC)
    set_target_properties(worldengine-cli PROPERTIES LINK_FLAGS "/ignore:4099")
endif()
