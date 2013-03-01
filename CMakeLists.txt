
# :mode=cmake:noTabs=true:

cmake_minimum_required(VERSION 2.8)

find_path(irrlicht_include
    NAMES irrlicht.h
    HINTS /usr/include/irrlicht
)

find_library(irrlicht_lib
    NAMES Irrlicht
)

include_directories(${irrlicht_include})

project(NodeBoxGenerator CXX)

set(NBGEN_SRC
    main.cpp
    cNodes.cpp
    CommonFunctions.cpp
    cEditor.cpp
)

add_executable(nbgen_exe ${NBGEN_SRC})

set_target_properties(nbgen_exe
    PROPERTIES
    OUTPUT_NAME NodeBoxGenerator
)

target_link_libraries(nbgen_exe ${irrlicht_lib})
