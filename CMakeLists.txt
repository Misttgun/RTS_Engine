cmake_minimum_required(VERSION 3.5)

project(RTS_Engine)

# Organize CMake folders (ZERO_CHECKS and ALL_BUILD)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PREDEFINED_TARGETS_FOLDER "CustomTargets")

# Set the executable output path
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#Bring the headers, such as Student.h into the project
include_directories(include)

#Add all the SFML libs
file(GLOB SFML_LIBS "lib/SFML-2.5.1/bin/*.dll")

#Add the sources
file(GLOB SOURCES "src/*.cpp")

## If you want to link SFML statically
# set(SFML_STATIC_LIBRARIES TRUE)

# Set SFML directory
set(SFML_DIR "lib/SFML-2.5.1/lib/cmake/SFML")

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(RTS_Engine ${SOURCES})

# Copy SFML libs and ressources to Output path
add_custom_command (
    TARGET RTS_Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${SFML_LIBS} ${CMAKE_BINARY_DIR}/$<CONFIG>
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/$<CONFIG>/res"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/res" ${CMAKE_BINARY_DIR}/$<CONFIG>/res
)

target_link_libraries(RTS_Engine sfml-graphics sfml-audio)