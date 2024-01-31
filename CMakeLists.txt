cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(GD-Roulette LANGUAGES CXX VERSION 1.0.0)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

file(GLOB_RECURSE SRC_MAIN
    "${SRC_DIR}/**.hpp"
    "${SRC_DIR}/**.cpp"
)
add_library(${PROJECT_NAME} MODULE "${SRC_MAIN}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

source_group(TREE ${SRC_DIR} PREFIX "src" FILES ${SRC_MAIN} )

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)


setup_geode_mod(${PROJECT_NAME})

# Get needed modules
include(scripts/git.cmake)

message("Getting modules\n")
include(scripts/json.cmake)
include(scripts/curl.cmake)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${json_SOURCE_DIR}/single_include
    ${curl_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} libcurl_static)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${SRC_DIR}/pch.hpp)

# Warnings as errors
set_property(TARGET ${PROJECT_NAME} PROPERTY COMPILE_WARNING_AS_ERROR ON)

add_compile_definitions(
    "_CRT_SECURE_NO_WARNINGS"
    "NOMINMAX"
    "WIN32_LEAN_AND_MEAN"
)
