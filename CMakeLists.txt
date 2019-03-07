cmake_minimum_required(VERSION 3.5)

project(RTS_Engine)

# Organize CMake folders (ZERO_CHECKS and ALL_BUILD)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PREDEFINED_TARGETS_FOLDER "CustomTargets")

# Set the executable output path
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#Bring the headers into the project
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/ecs)
include_directories(${CMAKE_SOURCE_DIR}/src/state)
include_directories(${CMAKE_SOURCE_DIR}/src/utility)
include_directories(${CMAKE_SOURCE_DIR}/src/window)
include_directories(${CMAKE_SOURCE_DIR}/src/threading)
include_directories(${CMAKE_SOURCE_DIR}/src/gui)
include_directories(${CMAKE_SOURCE_DIR}/src/input)
include_directories(${CMAKE_SOURCE_DIR}/src/message)
include_directories(${CMAKE_SOURCE_DIR}/src/game)

#Add the headers
file(GLOB_RECURSE HEADERS "src/*.h")

#Add the sources
file(GLOB_RECURSE SOURCES "src/*.cpp")

#Link SFML statically
set(SFML_STATIC_LIBRARIES TRUE)

# Set SFML directory
set(SFML_DIR "lib/SFML/lib/cmake/SFML")

find_package(SFML 2.5 COMPONENTS network audio graphics window system REQUIRED)

add_executable(RTS_Engine ${SOURCES} ${HEADERS})

# Copy SFML libs and ressources to Output path
add_custom_command (
    TARGET RTS_Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/res" ${CMAKE_BINARY_DIR}/$<CONFIG>/res
)

target_link_libraries(RTS_Engine sfml-network sfml-audio sfml-graphics sfml-window sfml-system)