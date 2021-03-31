cmake_minimum_required(VERSION 3.19)
project(libworldengine-test CXX)

include(GoogleTest)

find_package(Boost)
find_package(GTest)
find_package(Protobuf)

set(SRC_MAIN source/BasicTest.cpp
             source/PathTest.cpp
             source/WorldEngineTest.cpp)
set(SRC_SIMULATIONS source/simulations/TemperatureTest.cpp)

add_executable(worldengine-test ${SRC_MAIN}
                                ${SRC_SIMULATIONS})

source_group("Source Files\\main"        FILES ${SRC_MAIN})
source_group("Source Files\\simulations" FILES ${SRC_SIMULATIONS})

target_include_directories(worldengine-test PRIVATE ${GTest_INCLUDE_DIRS})

set_target_properties(worldengine-test PROPERTIES CXX_STANDARD 17
                                                  CXX_STANDARD_REQUIRED ON
                                                  CXX_EXTENSIONS OFF)

gtest_discover_tests(worldengine-test)

target_link_libraries(worldengine-test Boost::log
                                       GTest::gtest
                                       OpenSimplexNoise
                                       protobuf::protobuf
                                       worldengine)
