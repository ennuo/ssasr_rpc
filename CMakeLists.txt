cmake_minimum_required(VERSION 3.25.2)
project(se_editor VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin> CACHE INTERNAL "")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

file(GLOB_RECURSE SRC_FILES 
	"${PROJECT_SOURCE_DIR}/code/*.cpp"
	"${PROJECT_SOURCE_DIR}/sdk/imgui/*.cpp"
)

file(GLOB_RECURSE HEADER_FILES 
	"${PROJECT_SOURCE_DIR}/code/*.h" 
	"${PROJECT_SOURCE_DIR}/sdk/stb/*.h"
	"${PROJECT_SOURCE_DIR}/sdk/imgui/*.h"
)

add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC 
	"${PROJECT_SOURCE_DIR}/code" 
	"${PROJECT_SOURCE_DIR}/sdk/stb"
	"${PROJECT_SOURCE_DIR}/sdk/imgui"
)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_definitions(SE_EDITOR_DEBUG)
	if (MINGW)
		target_link_libraries(${PROJECT_NAME} PRIVATE -mconsole)
	endif()
endif()
if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_subdirectory(sdk/glm)
add_subdirectory(sdk/glfw)
add_subdirectory(sdk/glad)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC glm glfw glad)
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC PC=1 SE_EDITOR_VERSION="${CMAKE_PROJECT_VERSION}")