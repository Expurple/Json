cmake_minimum_required(VERSION 3.16)

project(ExpurpleJson VERSION 0.1.0 LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 17)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wall -Wextra)
endif()


add_subdirectory(src)

# for other subfolders
include_directories(src)


option(EXPURPLE_JSON_BUILD_TESTING OFF)
if(EXPURPLE_JSON_BUILD_TESTING)
    include(ExternalProject)
    find_package(Git REQUIRED)
    ExternalProject_Add(
        doctest
        PREFIX ${CMAKE_SOURCE_DIR}/externals/doctest
        GIT_REPOSITORY https://github.com/onqtam/doctest.git
        TIMEOUT 10
        UPDATE_COMMAND ${GIT_EXECUTABLE} pull
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_DOWNLOAD ON
    )
    ExternalProject_Get_Property(doctest source_dir)
    set(DOCTEST_INCLUDE_DIR ${source_dir}/doctest CACHE INTERNAL "Path to include folder for doctest")

    add_subdirectory(tests)
endif()
