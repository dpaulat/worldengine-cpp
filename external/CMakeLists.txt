cmake_minimum_required(VERSION 3.19)
set(PROJECT_NAME external)

# Plate Tectonics is LGPL, force shared library
set(TMP_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS True)
add_subdirectory(plate-tectonics)
if (MSVC)
    target_sources(PlateTectonics PRIVATE PlateTectonics.def)
endif()
set(BUILD_SHARED_LIBS ${TMP_BUILD_SHARED_LIBS})
unset(TMP_BUILD_SHARED_LIBS)
