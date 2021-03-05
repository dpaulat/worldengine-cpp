cmake_minimum_required(VERSION 3.19)
set(PROJECT_NAME worldengine)
project (${PROJECT_NAME} CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
conan_basic_setup(TARGETS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_ALL_NO_LIB -DBOOST_NUMPY_STATIC_LIB -DBOOST_PYTHON_STATIC_LIB")

set(WORLDENGINE_DIR ${PROJECT_SOURCE_DIR})
set(PYTHON_INCLUDE_DIR $ENV{PYTHON_INCLUDE})
set(PYTHON_LIBS_DIR $ENV{PYTHON_LIBS})

# TODO: Plate Tectonics is LGPL, force shared library
# Needs modification for Visual Studio, no dllexports exist
add_subdirectory(external/plate-tectonics)

add_subdirectory(worldengine)
add_subdirectory(cli)
