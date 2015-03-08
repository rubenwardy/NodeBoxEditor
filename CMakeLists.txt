#
# Set up project
#
cmake_minimum_required(VERSION 2.6)
project(NodeBoxEditor)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

#
# Configuration
#
set(NBE_MAJOR_VERSION 0)
set(NBE_MINOR_VERSION 8)
set(NBE_PATCH_VERSION 1)
set(NBE_DESCR_VERSION "0.8.1 - Glass")
configure_file(
	"${PROJECT_SOURCE_DIR}/src/conf_cmake.hpp.in"
	"${PROJECT_BINARY_DIR}/conf_cmake.hpp"
)

#
# Source files
#
set(NBE_SRC
	src/main.cpp
	src/Configuration.cpp
	src/GUIHelpers.cpp
	src/EditorState.cpp
	src/MenuState.cpp
	src/Editor.cpp
	src/modes/NBEditor.cpp
	src/modes/NodeEditor.cpp
	src/modes/TextureEditor.cpp
	src/dialogs/FileDialog.cpp
	src/dialogs/TextureDialog.cpp
	src/dialogs/ImageDialog.cpp
	src/Project.cpp
	src/MediaManager.cpp
	src/Node.cpp
	src/NodeBox.cpp
	src/minetest.cpp
	src/FileFormat/FileFormat.cpp
	src/FileFormat/helpers.cpp
	src/FileFormat/NBE.cpp
	src/FileFormat/Lua.cpp
	src/FileFormat/CPP.cpp
	src/FileFormat/obj.cpp
	src/util/string.cpp
	src/util/filesys.cpp
	src/util/SimpleFileCombiner.cpp
	src/util/tinyfiledialogs.c
)
add_executable(${PROJECT_NAME} ${NBE_SRC})

#
# Dependencies
#
find_package(Irrlicht REQUIRED)
find_package(ZLIB REQUIRED)
find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(JPEG REQUIRED)
find_package(BZip2 REQUIRED)
find_package(PNG REQUIRED)
include_directories(
	${PROJECT_BINARY_DIR}
	${IRRLICHT_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIR}
	${CMAKE_BUILD_TYPE}
	${X11_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIR}
	${PNG_INCLUDE_DIR}
)
set(TLL
	${IRRLICHT_LIBRARY}
	${ZLIB_LIBRARIES}
	${X11_LIBRARIES}
	${OPENGL_LIBRARIES}
	${JPEG_LIBRARIES}
	${BZIP2_LIBRARIES}
	${PNG_LIBRARIES}
)
if(UNIX)
	find_library(XXF86VM_LIBRARY Xxf86vm)
	target_link_libraries(
		${PROJECT_NAME}
		${TLL}
		${XXF86VM_LIBRARY}
	)
else()
	target_link_libraries(
		${PROJECT_NAME}
		${TLL}
	)
endif(UNIX)

#
# Executable
#
file(MAKE_DIRECTORY "bin")
SET(CMAKE_CXX_FLAGS
	"${CMAKE_CXX_FLAGS} -pthread")
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    OUTPUT_NAME bin/nodeboxeditor
)

#
# Installation
#
install(FILES   media/fontlucida.png        DESTINATION share/nodeboxeditor/media)
install(FILES   media/gui_scale.png         DESTINATION share/nodeboxeditor/media)
install(FILES   media/coordinates.png       DESTINATION share/nodeboxeditor/media)
install(FILES   media/icon_mode_node.png    DESTINATION share/nodeboxeditor/media)
install(FILES   media/icon_mode_nodebox.png DESTINATION share/nodeboxeditor/media)
install(FILES   media/icon_mode_texture.png DESTINATION share/nodeboxeditor/media)
install(FILES   media/sky.jpg               DESTINATION share/nodeboxeditor/media)
install(FILES   media/texture_box.png       DESTINATION share/nodeboxeditor/media)
install(FILES   media/texture_terrain.png   DESTINATION share/nodeboxeditor/media)
install(FILES   media/icon.png              DESTINATION share/nodeboxeditor/media)
install(FILES   media/rotate_x.png          DESTINATION share/nodeboxeditor/media)
install(FILES   media/rotate_y.png          DESTINATION share/nodeboxeditor/media)
install(FILES   media/rotate_z.png          DESTINATION share/nodeboxeditor/media)
install(FILES   editor.conf.example         DESTINATION share/nodeboxeditor)
if(UNIX)
	install (FILES   nodeboxeditor.desktop       DESTINATION ~/.local/share/applications)
endif()
install(FILES   README.md                   DESTINATION share/nodeboxeditor)
install(TARGETS ${PROJECT_NAME}             DESTINATION bin)
if(WIN32)
	if(DEFINED IRRLICHT_DLL)
		install(FILES ${IRRLICHT_DLL} DESTINATION bin)
	endif()
endif()
