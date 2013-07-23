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

# Source files
set(NBGEN_SRC
	main.cpp
	Editor.cpp
	Node.cpp
	NBPropertiesDialog.cpp
	CodeDialog.cpp
	CommonFunctions.cpp
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
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    OUTPUT_NAME NodeBoxGenerator
)

# Install DLLs
if(WIN32)
	if(DEFINED IRRLICHT_DLL)
		message(Installing irrlicht dll)
		install(FILES ${IRRLICHT_DLL})
	endif()
endif()
