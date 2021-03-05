cmake_minimum_required(VERSION 3.19)
project(worldengine-cli CXX)

add_executable(worldengine source/main.cpp
                           source/worldengine.cpp
                           source/worldengine.h)

target_include_directories(worldengine PRIVATE ${PYTHON_INCLUDE_DIR})
target_include_directories(worldengine PRIVATE ${libworldengine_SOURCE_DIR}/source)
target_link_directories(worldengine PRIVATE ${PYTHON_LIBS_DIR})
target_link_libraries(worldengine python3 libworldengine)

find_package(Boost COMPONENTS log
                              numpy39
                              program_options REQUIRED)

target_link_libraries(worldengine Boost::log
                                  Boost::numpy39
                                  Boost::program_options)
