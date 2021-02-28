cmake_minimum_required(VERSION 3.19)
set(PROJECT_NAME worldengine)
project (${PROJECT_NAME} CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(WORLDENGINE_DIR ${PROJECT_SOURCE_DIR})

add_subdirectory(external/plate-tectonics)
add_subdirectory(worldengine)
