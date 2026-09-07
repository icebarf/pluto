cmake_minimum_required(VERSION 3.11)
project(pdp1120 CXX)

file(GLOB_RECURSE PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 26)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 26)