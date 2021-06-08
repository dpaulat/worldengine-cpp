cmake_minimum_required(VERSION 3.11)
project(libworldengine-test CXX)

include(GoogleTest)

find_package(Boost)
find_package(GTest)
find_package(HDF5)
find_package(Protobuf)

set(SRC_MAIN source/WorldEngineTest.cpp)
set(HDR_SUPPORT source/Functions.h)
set(SRC_SUPPORT source/Functions.cpp)
set(SRC_TESTS source/BasicTest.cpp
              source/GenerationTest.cpp
              source/ImageTest.cpp
              source/PathTest.cpp
              source/SerializationTest.cpp
              source/SimulationTest.cpp)

add_executable(worldengine-test ${SRC_MAIN}
                                ${HDR_SUPPORT}
                                ${SRC_SUPPORT}
                                ${SRC_TESTS})

source_group("Source Files\\main"    FILES ${SRC_MAIN})
source_group("Header Files\\support" FILES ${HDR_SUPPORT})
source_group("Source Files\\support" FILES ${SRC_SUPPORT})
source_group("Source Files\\tests"   FILES ${SRC_TESTS})

target_include_directories(worldengine-test PRIVATE ${GTest_INCLUDE_DIRS}
                                                    ${PNG_INCLUDE_DIR}
                                                    ${libworldengine_SOURCE_DIR}/source)

set_target_properties(worldengine-test PROPERTIES CXX_STANDARD 17
                                                  CXX_STANDARD_REQUIRED ON
                                                  CXX_EXTENSIONS OFF)

if (MSVC)
    set_target_properties(worldengine-test PROPERTIES LINK_FLAGS "/ignore:4099")
endif()

target_compile_definitions(worldengine-test PRIVATE WORLDENGINE_TEST_DATA_DIR="${WORLDENGINE_DIR}/external/worldengine-cpp-data/tests")

gtest_discover_tests(worldengine-test)

target_link_libraries(worldengine-test Boost::log
                                       GTest::gtest
                                       HDF5::HDF5
                                       protobuf::protobuf
                                       PNG::PNG
                                       worldengine
                                       OpenSimplexNoise
                                       PlateTectonics)
