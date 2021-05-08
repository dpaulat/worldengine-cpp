cmake_minimum_required(VERSION 3.19)
project(libworldengine-test CXX)

include(GoogleTest)

find_package(Boost)
find_package(GTest)
find_package(HDF5)
find_package(Protobuf)

set(SRC_MAIN source/BasicTest.cpp
             source/GenerationTest.cpp
             source/PathTest.cpp
             source/SerializationTest.cpp
             source/SimulationTest.cpp
             source/WorldEngineTest.cpp)

add_executable(worldengine-test ${SRC_MAIN})

source_group("Source Files\\main" FILES ${SRC_MAIN})

target_include_directories(worldengine-test PRIVATE ${GTest_INCLUDE_DIRS}
                                                    ${libworldengine_SOURCE_DIR}/source)

set_target_properties(worldengine-test PROPERTIES CXX_STANDARD 17
                                                  CXX_STANDARD_REQUIRED ON
                                                  CXX_EXTENSIONS OFF)

gtest_discover_tests(worldengine-test)

target_link_libraries(worldengine-test Boost::log
                                       GTest::gtest
                                       HDF5::HDF5
                                       OpenSimplexNoise
                                       protobuf::protobuf
                                       PlateTectonics
                                       worldengine)
