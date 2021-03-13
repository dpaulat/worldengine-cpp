cmake_minimum_required(VERSION 3.19)
project(worldengine-docs)

find_package(Doxygen)

if (DOXYGEN_FOUND)
    find_package(Sphinx REQUIRED)

    # Exclude patterns
    set(DOXYGEN_EXCLUDE_PATTERNS
        */build*/*
        */external/*
    )
    
    # Doxygen settings
    # - Project
    set(DOXYGEN_PROJECT_NAME "WorldEngine C++")
    set(DOXYGEN_OUTPUT_DIRECTORY ${worldengine_BINARY_DIR}/docs/doxygen)
    # - Build
    set(DOXYGEN_SHOW_GROUPED_MEMB_INC YES)
    # - XML
    set(DOXYGEN_GENERATE_XML YES)
    set(DOXYGEN_XML_NS_MEMB_FILE_SCOPE YES)
    
    # This target will only be built if specifically asked to
    doxygen_add_docs(
        doxygen
        ${worldengine_SOURCE_DIR}
        COMMENT "Generating doxygen documentation")

    set(SPHINX_SOURCE ${PROJECT_SOURCE_DIR})
    set(SPHINX_BUILD ${worldengine_BINARY_DIR}/docs/sphinx)

    add_custom_target(sphinx ALL
                      COMMAND ${SPHINX_EXECUTABLE}
                      -Dbreathe_projects.worldengine=${DOXYGEN_OUTPUT_DIRECTORY}/xml
                      ${SPHINX_SOURCE} ${SPHINX_BUILD}
                      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                      DEPENDS ${PROJECT_SOURCE_DIR}/index.rst
                              ${PROJECT_BINARY_DIR}/Doxyfile.doxygen
                      MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
                      COMMENT "Generating Sphinx documentation")
endif(DOXYGEN_FOUND)
