cmake_minimum_required(VERSION 3.19)
set(PROJECT_NAME worldengine)
project (${PROJECT_NAME} CXX)

enable_testing()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include(${PROJECT_SOURCE_DIR}/external/cmake-conan/conan.cmake)

conan_cmake_configure(REQUIRES boost/1.75.0
                               eigen/3.3.9
                               gtest/1.10.0
                               libpng/1.6.37
                               protobuf/3.15.5
                      GENERATORS cmake
                                 cmake_find_package
                                 cmake_paths
                      BUILD_REQUIRES cmake/3.19.4
                      IMPORTS "bin, *.dll -> ./bin"
                      IMPORTS "lib, *.dylib -> ./bin")

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conan-center
                    SETTINGS ${settings})

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
conan_basic_setup(TARGETS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_ALL_NO_LIB")

set(WORLDENGINE_DIR ${PROJECT_SOURCE_DIR})

add_subdirectory(external)
add_subdirectory(worldengine)
add_subdirectory(cli)
add_subdirectory(docs)
