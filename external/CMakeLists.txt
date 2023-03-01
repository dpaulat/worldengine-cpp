cmake_minimum_required(VERSION 3.11)
set(PROJECT_NAME external)

# Plate Tectonics is LGPL, force shared library
set(PLATE_TECTONICS_INCLUDE_DIR ${WORLDENGINE_DIR}/external/plate-tectonics/src PARENT_SCOPE)
set(TMP_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS True)
set(TMP_WITH_TESTS ${WITH_TESTS})
set(WITH_TESTS False)
add_subdirectory(plate-tectonics)
if (MSVC)
    target_sources(PlateTectonics PRIVATE ${WORLDENGINE_DIR}/external/PlateTectonics.def)
endif()
set(BUILD_SHARED_LIBS ${TMP_BUILD_SHARED_LIBS})
set(WITH_TESTS ${TMP_WITH_TESTS})
unset(TMP_BUILD_SHARED_LIBS)
unset(TMP_WITH_TESTS)
set_target_properties(PlateTectonics PROPERTIES CXX_STANDARD 11
                                                CXX_STANDARD_REQUIRED ON
                                                CXX_EXTENSIONS OFF)

# OpenSimplexNoise has an UNLICENSE
if (MSVC)
    add_definitions(/FI"cstdint")
else()
    add_definitions(-include cstdint)
endif()
set(OPENSIMPLEX_NOISE_INCLUDE_DIR ${WORLDENGINE_DIR}/external/OpenSimplexNoise/OpenSimplexNoise PARENT_SCOPE)
add_library(OpenSimplexNoise OpenSimplexNoise/OpenSimplexNoise/OpenSimplexNoise.cpp
                             OpenSimplexNoise/OpenSimplexNoise/OpenSimplexNoise.h)
set_target_properties(OpenSimplexNoise PROPERTIES CXX_STANDARD 11
                                                  CXX_STANDARD_REQUIRED ON
                                                  CXX_EXTENSIONS OFF)
