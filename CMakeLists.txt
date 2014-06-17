# Set up project
cmake_minimum_required(VERSION 2.6)
project(NodeBoxGenerator)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Dependancies
find_package(Irrlicht REQUIRED)
find_package(ZLIB REQUIRED)
find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(JPEG REQUIRED)
find_package(BZip2 REQUIRED)
find_package(PNG REQUIRED)
find_library(XXF86VM_LIBRARY Xxf86vm)

# Source files
set(NBGEN_SRC
	src/main.cpp
	src/Configuration.cpp
	src/Editor.cpp
	src/EditorState.cpp
	src/MenuState.cpp
	src/GUIHelpers.cpp
	src/NBEditor.cpp
	src/NodeEditor.cpp
	src/FileFormat/FileFormat.cpp
	src/FileFormat/NBE.cpp
	src/FileFormat/Lua.cpp	
	src/FileFormat/MTC.cpp
	src/Node.cpp
	src/NodeBox.cpp
	src/Project.cpp
	src/util/string.cpp
)

# Include Directories
include_directories(
	${PROJECT_BINARY_DIR}
	${IRRLICHT_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIR}
	${CMAKE_BUILD_TYPE}
	${X11_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIR}
	${PNG_INCLUDE_DIR}
)

add_executable(${PROJECT_NAME} ${NBGEN_SRC})

target_link_libraries(
	${PROJECT_NAME}
	${IRRLICHT_LIBRARY}
	${ZLIB_LIBRARIES}
	${X11_LIBRARIES}
	${OPENGL_LIBRARIES}
	${JPEG_LIBRARIES}
	${BZIP2_LIBRARIES}
	${PNG_LIBRARIES}
	${XXF86VM_LIBRARY}
)

file(MAKE_DIRECTORY "bin")

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    OUTPUT_NAME bin/nodeboxeditor
)

# Install DLLs
if(WIN32)
	if(DEFINED IRRLICHT_DLL)
		message(Installing irrlicht dll)
		install(FILES ${IRRLICHT_DLL})
	endif()
endif()
