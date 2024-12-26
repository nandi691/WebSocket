cmake_minimum_required(VERSION 3.10)

# Project name
project(MyProject)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/Common)

# Add all source files except those in "coding-practice" and "CMakeFiles"
file(GLOB_RECURSE SOURCES 
    ${CMAKE_SOURCE_DIR}/*.c
    ${CMAKE_SOURCE_DIR}/*.cpp
)

# Exclude "coding-practice" directory and "CMakeFiles" directory
foreach(FILE IN LISTS SOURCES)
    if(FILE MATCHES "/coding-practice/" OR FILE MATCHES "/CMakeFiles/")
        list(REMOVE_ITEM SOURCES ${FILE})
    endif()
endforeach()

# Specify the output directory for the executable
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/exe)

# Add executable with the desired name
add_executable(WebSocket ${SOURCES})
