cmake_minimum_required(VERSION 3.19)
set(PROJECT_NAME worldengine)
project (${PROJECT_NAME} CXX)

include(${PROJECT_SOURCE_DIR}/external/cmake-conan/conan.cmake)

conan_cmake_configure(REQUIRES boost/1.75.0
                               gtest/1.10.0
                      GENERATORS cmake
                                 cmake_find_package
                                 cmake_paths
                      BUILD_REQUIRES cmake/3.19.4
                      IMPORTS "bin, *.dll -> ./bin"
                      IMPORTS "lib, *.dylib -> ./bin"
                      OPTIONS boost:without_python=False)

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conan-center
                    SETTINGS ${settings})

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
conan_basic_setup(TARGETS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_ALL_NO_LIB -DBOOST_NUMPY_STATIC_LIB -DBOOST_PYTHON_STATIC_LIB")

set(WORLDENGINE_DIR ${PROJECT_SOURCE_DIR})
set(PYTHON_INCLUDE_DIR $ENV{PYTHON_INCLUDE})
set(PYTHON_LIBS_DIR $ENV{PYTHON_LIBS})

add_subdirectory(external)
add_subdirectory(worldengine)
add_subdirectory(cli)
