cmake_minimum_required(VERSION 3.19)
project(worldengine-cli CXX)

add_executable(worldengine source/main.cpp)

target_include_directories(worldengine PRIVATE ${PYTHON_INCLUDE_DIR})
target_include_directories(worldengine PRIVATE ${libworldengine_SOURCE_DIR}/source)
target_link_directories(worldengine PRIVATE ${PYTHON_LIBS_DIR})
target_link_libraries(worldengine python3 libworldengine)

find_package(Boost)

if(BOOST_FOUND)
    set(WORLDENGINE_BOOST_LIBS libboost_system
                               libboost_log
                               libboost_python39
                               libboost_numpy39
                               libboost_thread)

    target_link_libraries(worldengine ${WORLDENGINE_BOOST_LIBS})
endif()