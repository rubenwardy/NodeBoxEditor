# Set up project
cmake_minimum_required(VERSION 2.6)
project(NodeBoxEditor)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Configuration
option (RUN_IN_PLACE
	"Whether editor should run in place (see readme.md)" ON) 
IF (RUN_IN_PLACE)
	set (BOOL_RUN_IN_PLACE true)
ELSE (RUN_IN_PLACE)	
	set (BOOL_RUN_IN_PLACE false)
ENDIF (RUN_IN_PLACE)
set(NBE_MAJOR_VERSION 0)
set(NBE_MINOR_VERSION 6)
set(NBE_PATCH_VERSION 4)
set(NBE_DESCR_VERSION "\"0.6.4 - Stone\"")
configure_file (
	src/common.hpp.in
	src/common.hpp
)

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
if (NOT RUN_IN_PLACE)
	install (FILES media/fontlucida.png DESTINATION nbe/media)
	install (FILES media/gui_scale.png DESTINATION nbe/media)
	install (FILES media/icon_mode_node.png DESTINATION nbe/media)
	install (FILES media/icon_mode_nodebox.png DESTINATION nbe/media)
	install (FILES media/sky.jpg DESTINATION nbe/media)
	install (FILES media/texture_box.png DESTINATION nbe/media)
	install (FILES media/texture_terrain.png DESTINATION nbe/media)
	install (FILES editor.conf.example DESTINATION nbe)
	install (FILES README.md DESTINATION nbe)
	install (TARGETS ${PROJECT_NAME} DESTINATION bin)
endif (NOT RUN_IN_PLACE)

if(WIN32)
	if(DEFINED IRRLICHT_DLL)
		message(Installing irrlicht dll)
		install(FILES ${IRRLICHT_DLL} DESTINATION bin)
	endif()
endif()
