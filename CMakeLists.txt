cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
cmake_policy(VERSION 2.8)
if (NOT CMAKE_VERSION VERSION_LESS "3.1")
	cmake_policy(SET CMP0054 NEW)
endif()

set(OGL_SAMPLES_NAME ogl-samples)
set(FRAMEWORK_NAME framework)

project(${OGL_SAMPLES_NAME})
enable_testing()

#CMAKE_BUILD_TOOL

if(CMAKE_CURRENT_SOURCE_DIR EQUAL CMAKE_CURRENT_BINARY_DIR)
	message(FATAL_ERROR "Source and binary directories must be different")
endif(CMAKE_CURRENT_SOURCE_DIR EQUAL CMAKE_CURRENT_BINARY_DIR)

if(MSVC)
	set(BINARY_DIRECTORY build)
endif()

if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel") AND UNIX))
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
	set(CMAKE_CXX_FLAGS "-std=c++11")
endif()

option(OGL_SAMPLES_IN_SOURCE_BUILD "OGL_SAMPLES_IN_SOURCE_BUILD" OFF)
if(OGL_SAMPLES_IN_SOURCE_BUILD)
	add_definitions(-DOGL_SAMPLES_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
	add_definitions(-DOGL_SAMPLES_BINARY_DIR="../")
else()
	add_definitions(-DOGL_SAMPLES_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
	add_definitions(-DOGL_SAMPLES_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")
endif()

################################
# Add GLEW

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)
add_definitions(-DGLEW_STATIC)

################################
# Add GLFW

set(GLFW_DIRECTORY glfw-3.1.1)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/${GLFW_DIRECTORY}/include)

set(GLFW_INSTALL OFF CACHE STRING "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE STRING "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE STRING "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/${GLFW_DIRECTORY})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/${GLFW_DIRECTORY}/include)

################################
# Add freeimage 

if(MSVC AND NOT CMAKE_CL_64)
	set(FREEIMAGE_BINARY_DIRECTORY "win32")
endif()

if(MSVC AND CMAKE_CL_64)
	set(FREEIMAGE_BINARY_DIRECTORY "win64")
endif()

if(MINGW)
  set(FREEIMAGE_BINARY_DIRECTORY "win64")
  MESSAGE(WARNING "x64 version of FREEIMAGE is used by default")
endif()

if(APPLE)
	set(FREEIMAGE_BINARY_DIRECTORY "macosx")
endif()

if(UNIX AND NOT APPLE AND CMAKE_CL_64)
	set(FREEIMAGE_BINARY_DIRECTORY "linux64")
endif()

if(UNIX AND NOT APPLE AND NOT CMAKE_CL_64)
	set(FREEIMAGE_BINARY_DIRECTORY "linux64")
endif()

set(FREEIMAGE_DIRECTORY FreeImage-3.15.4)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/${FREEIMAGE_DIRECTORY})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/${FREEIMAGE_DIRECTORY}/${FREEIMAGE_BINARY_DIRECTORY})

if(MSVC OR MINGW)
	set(FREEIMAGE_LIBRARY FreeImage.lib)
	set(FREEIMAGE_BINARY_FILE FreeImage.dll)
else()
	add_definitions(-DFREEIMAGE_LIB)
	set(FREEIMAGE_LIBRARY freeimage)
	set(FREEIMAGE_BINARY_FILE freeimage)
endif()

set(FREEIMAGE_BIN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/${FREEIMAGE_DIRECTORY}/${FREEIMAGE_BINARY_DIRECTORY}/${FREEIMAGE_BINARY_FILE})

################################
# Add external library

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)

################################
# Add GLO

set(GLO_DIRECTORY glo-0.1.0.0)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/${GLO_DIRECTORY})

################################
# Add GLU

set(GLU_DIRECTORY glu-0.1.0.0)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/${GLU_DIRECTORY})

################################
# Add OpenGL ES

set(ES_DIRECTORY gles-2.0)

if(MSVC AND NOT CMAKE_CL_64)
	set(ES_BINARY_DIRECTORY "win32-vc")
endif(MSVC AND NOT CMAKE_CL_64)

if(MSVC AND CMAKE_CL_64)
	set(ES_BINARY_DIRECTORY "win64-vc")
endif(MSVC AND CMAKE_CL_64)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/${ES_DIRECTORY})
if(WIN32)
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/${ES_DIRECTORY}/lib/${ES_BINARY_DIRECTORY})
endif()

set(ES_LIBRARY libGLESv2)

################################
# Add EGL

set(EGL_DIRECTORY egl-1.4)

if(MSVC AND NOT CMAKE_CL_64)
	set(EGL_BINARY_DIRECTORY "win32-vc")
endif(MSVC AND NOT CMAKE_CL_64)

if(MSVC AND CMAKE_CL_64)
	set(EGL_BINARY_DIRECTORY "win64-vc")
endif(MSVC AND CMAKE_CL_64)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/${EGL_DIRECTORY})
if(WIN32)
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/${EGL_DIRECTORY}/lib/${EGL_BINARY_DIRECTORY})
endif()

set(EGL_LIBRARY libEGL)

################################
# Add libraries to executables

set(BINARY_FILES glfw ${GLFW_LIBRARIES} ${FREEIMAGE_LIBRARY})

################################
# Add output directory

if(MSVC)
	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/build/)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(COPY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/build/Debug/ )
	else(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(COPY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/build/Release/ )
	endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
else(MSVC)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/build/Debug )
	else(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/build/Release )
	endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
endif(MSVC)

################################
# Add options

option(OGL_SAMPLES_AUTOMATED_TESTS "OGL_SAMPLES_AUTOMATED_TESTS" OFF)
if(OGL_SAMPLES_AUTOMATED_TESTS)
	add_definitions(-DAUTOMATED_TESTS)
endif()

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
	if (NOT APPLE)
		add_definitions(-fpermissive)
	endif()
endif()
################################
# Add definitions

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

################################
# Add framework

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/framework)
add_subdirectory(framework)

################################
# Copy binary

if(MSVC OR MINGW)
	set(COPY_BINARY copy-binary)
	add_custom_target(${COPY_BINARY}
		COMMAND ${CMAKE_COMMAND} -E copy ${FREEIMAGE_BIN_PATH} ${CMAKE_CURRENT_BINARY_DIR}/samples/${FREEIMAGE_BINARY_FILE}
		COMMAND ${CMAKE_COMMAND} -E copy ${FREEIMAGE_BIN_PATH} ${CMAKE_CURRENT_BINARY_DIR}/build/Debug/${FREEIMAGE_BINARY_FILE}
		COMMAND ${CMAKE_COMMAND} -E copy ${FREEIMAGE_BIN_PATH} ${CMAKE_CURRENT_BINARY_DIR}/build/Release/${FREEIMAGE_BINARY_FILE})
endif()

################################
# Add subdirectory

add_subdirectory(samples)

################################
# Add install

install (DIRECTORY data DESTINATION .)

# See http://www.vtk.org/Wiki/CMake:CPackPackageGenerators
set (CPACK_PACKAGE_VERSION_MAJOR "4")
set (CPACK_PACKAGE_VERSION_MINOR "5")
set (CPACK_PACKAGE_VERSION_PATCH "4")
# Use current date in YYYYMMDD format as patch number

# cpack mistakenly detects Mingw-w64 as win32
if (MINGW)
	if (CMAKE_SIZEOF_VOID_P EQUAL 8)
		set (CPACK_SYSTEM_NAME win64)
	endif ()
endif ()


if (WIN32)
	set (CPACK_GENERATOR "ZIP")
else ()
	set (CPACK_GENERATOR "TBZ2")
endif ()

include(CPack)
