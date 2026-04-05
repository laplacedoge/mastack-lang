cmake_minimum_required(VERSION 3.15)

project(mastack LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0)
endif()

add_subdirectory(3rd)
add_subdirectory(mastack)
add_subdirectory(test)