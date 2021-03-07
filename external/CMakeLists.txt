cmake_minimum_required(VERSION 3.19)
set(PROJECT_NAME external)

# Plate Tectonics is LGPL, force shared library
set(PLATE_TECTONICS_INCLUDE_DIR ${WORLDENGINE_DIR}/external/plate-tectonics/src PARENT_SCOPE)
set(TMP_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS True)
add_subdirectory(plate-tectonics)
if (MSVC)
    target_sources(PlateTectonics PRIVATE PlateTectonics.def)
endif()
set(BUILD_SHARED_LIBS ${TMP_BUILD_SHARED_LIBS})
unset(TMP_BUILD_SHARED_LIBS)

# OpenSimplexNoise has an UNLICENSE
if (MSVC)
    add_definitions(/FI"cstdint")
else()
    add_definitions(-include cstdint)
endif()
set(OPENSIMPLEX_NOISE_INCLUDE_DIR ${WORLDENGINE_DIR}/external/OpenSimplexNoise/OpenSimplexNoise PARENT_SCOPE)
add_library(OpenSimplexNoise OpenSimplexNoise/OpenSimplexNoise/OpenSimplexNoise.cpp
                             OpenSimplexNoise/OpenSimplexNoise/OpenSimplexNoise.h)
                             