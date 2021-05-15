cmake_minimum_required(VERSION 3.11)
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

    set(SPHINX_SOURCE_FILES ${SPHINX_SOURCE}/global.rst
                            ${SPHINX_SOURCE}/index.rst
                            ${SPHINX_SOURCE}/biomes.rst
                            ${SPHINX_SOURCE}/cli.rst
                            ${SPHINX_SOURCE}/contributing.rst
                            ${SPHINX_SOURCE}/gui.rst
                            ${SPHINX_SOURCE}/install.rst
                            ${SPHINX_SOURCE}/scenarios.rst)

    add_custom_target(sphinx ALL
                      COMMAND ${SPHINX_EXECUTABLE}
                      -Dbreathe_projects.worldengine=${DOXYGEN_OUTPUT_DIRECTORY}/xml
                      ${SPHINX_SOURCE} ${SPHINX_BUILD}
                      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                      DEPENDS ${SPHINX_SOURCE_FILES}
                              ${PROJECT_BINARY_DIR}/Doxyfile.doxygen
                      MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
                      COMMENT "Generating Sphinx documentation")

    add_dependencies(sphinx doxygen)

    set_target_properties(doxygen PROPERTIES EXCLUDE_FROM_ALL TRUE)
    set_target_properties(sphinx  PROPERTIES EXCLUDE_FROM_ALL TRUE)

endif(DOXYGEN_FOUND)
