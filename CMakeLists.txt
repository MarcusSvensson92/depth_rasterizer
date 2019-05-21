# Minimum required CMake version
cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)

# Project
project(DepthRasterizer)
    
# Set include directory
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/")

# Create executable
add_executable(${PROJECT_NAME} "DepthRasterizer.cpp")