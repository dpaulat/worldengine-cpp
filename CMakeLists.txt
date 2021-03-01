cmake_minimum_required(VERSION 3.19)
set(PROJECT_NAME worldengine)
project (${PROJECT_NAME} CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Boost)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_NUMPY_STATIC_LIB")

set(WORLDENGINE_DIR ${PROJECT_SOURCE_DIR})
set(PYTHON_INCLUDE_DIR $ENV{PYTHON_INCLUDE})
set(PYTHON_LIBS_DIR $ENV{PYTHON_LIBS})

add_subdirectory(external/plate-tectonics)
add_subdirectory(worldengine)
add_subdirectory(cli)
