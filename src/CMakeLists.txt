cmake_minimum_required(VERSION 3.16)

file(GLOB_RECURSE HEADERS "*.hpp")
file(GLOB_RECURSE SOURCES "*.cpp")

add_library(expurple-json SHARED ${HEADERS} ${SOURCES})
set_target_properties(expurple-json PROPERTIES VERSION ${PROJECT_VERSION})
