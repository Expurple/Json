cmake_minimum_required(VERSION 3.16)

include_directories(${DOCTEST_INCLUDE_DIR})
file(GLOB_RECURSE SOURCES "*.cpp")
add_executable(run-tests ${SOURCES})
target_link_libraries(run-tests expurple-json)
