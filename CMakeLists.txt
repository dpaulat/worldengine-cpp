cmake_minimum_required(VERSION 3.11)
set(PROJECT_NAME worldengine)
project (${PROJECT_NAME} CXX)

set(CMAKE_POLICY_DEFAULT_CMP0054 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

enable_testing()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include(${PROJECT_SOURCE_DIR}/external/cmake-conan/conan.cmake)

conan_cmake_configure(REQUIRES boost/1.81.0
                               eigen/3.4.0
                               gdal/3.5.2
                               gtest/cci.20210126
                               hdf5/1.13.1
                               libpng/1.6.39
                               protobuf/3.21.9
                               zlib/1.2.13 # Dependency override for conflict
                      GENERATORS cmake
                                 cmake_find_package
                                 cmake_paths
                      BUILD_REQUIRES cmake/3.19.4
                      IMPORTS "bin, *.dll -> ./bin"
                      IMPORTS "lib, *.dylib -> ./bin")

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
conan_basic_setup(TARGETS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_ALL_NO_LIB")

set(WORLDENGINE_DIR ${PROJECT_SOURCE_DIR})

option(BUILD_DOCS "Build documentation" ON)

add_subdirectory(external)
add_subdirectory(worldengine)
add_subdirectory(cli)

if(BUILD_DOCS)
    add_subdirectory(docs)
endif()
