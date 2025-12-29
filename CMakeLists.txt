cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${GEODE_TARGET_PLATFORM}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

if (DEFINED ENV{GITHUB_ACTIONS})
    set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "" FORCE)
endif()

project(GD-Roulette LANGUAGES CXX VERSION 1.0.0)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    # glob headers on Visual Studio so headers appear in the solution explorer view
    file(GLOB_RECURSE SRC_MAIN
        "${SRC_DIR}/**.hpp"
        "${SRC_DIR}/**.cpp"
    )
else()
    file(GLOB_RECURSE SRC_MAIN
        "${SRC_DIR}/**.cpp"
    )
endif()
add_library(${PROJECT_NAME} MODULE "${SRC_MAIN}")
target_include_directories(${PROJECT_NAME}
    PRIVATE ${SRC_DIR}/managers  # allow easy inclusion of managers
    PRIVATE ${SRC_DIR}/utils     # allow easy inclusion of utils
)

# make Visual Studio recognize folders as "filters"
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

source_group(TREE ${SRC_DIR} PREFIX "src" FILES ${SRC_MAIN})

# Platform specific stuff

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()


if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()
add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})

# Get needed modules
include(cmake/git.cmake)

message("Getting modules\n")
include(cmake/rtrp.cmake)

# Warnings as errors
set_property(TARGET ${PROJECT_NAME} PROPERTY COMPILE_WARNING_AS_ERROR ON)

# Build timestamp
string(TIMESTAMP CURRENT_TIME "%s")
add_definitions(-DROULETTE_BUILD_TIMESTAMP=${CURRENT_TIME})

# Debug macro
if (NOT DEFINED ENV{GITHUB_ACTIONS})
    target_compile_definitions(
        ${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:ROULETTE_DEBUG_BUILD>
            $<$<CONFIG:RelWithDebInfo>:ROULETTE_DEBUG_BUILD>
    )
endif()
