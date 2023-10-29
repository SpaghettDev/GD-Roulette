cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_GENERATOR_PLATFORM win32)

project(GD-Roulette CXX)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

# Get needed modules
include(scripts/git.cmake)

message("Getting modules\n")
include(scripts/cocos-headers.cmake)
include(scripts/fmod.cmake)
include(scripts/gd.h.cmake)
include(scripts/json.cmake)
include(scripts/curl.cmake)
include(scripts/minhook.cmake)

file(GLOB_RECURSE SRC_MAIN
    "${SRC_DIR}/**.hpp"
    "${SRC_DIR}/**.cpp"
)
add_library(GD-Roulette MODULE "${SRC_MAIN}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET GD-Roulette PROPERTY CXX_STANDARD 23)

source_group(TREE ${SRC_DIR} PREFIX "src" FILES ${SRC_MAIN} )

target_include_directories(GD-Roulette PRIVATE
    "${json_SOURCE_DIR}/single_include"
    "${gd.h_SOURCE_DIR}/include"
    "${gd.h_SOURCE_DIR}"
    "${fmod_SOURCE_DIR}/inc"
    "${cocos-headers_SOURCE_DIR}/cocos2dx"
    "${cocos-headers_SOURCE_DIR}/cocos2dx/include"
    "${cocos-headers_SOURCE_DIR}/cocos2dx/kazmath/include"
    "${cocos-headers_SOURCE_DIR}/cocos2dx/platform/win32"
    "${cocos-headers_SOURCE_DIR}/cocos2dx/platform/third_party/win32/OGLES"
    "${cocos-headers_SOURCE_DIR}/extensions"
)

# curl stuff
add_library(curl_int INTERFACE)
target_link_libraries(curl_int INTERFACE libcurl)
target_include_directories(curl_int INTERFACE ${curl_SOURCE_DIR}/include ${curl_BINARY_DIR}/include/curl)
add_library(CURL::libcurl ALIAS curl_int)

target_link_libraries(GD-Roulette PRIVATE
    minhook
    libcurl
    ${cocos-headers_SOURCE_DIR}/cocos2dx/libcocos2d.lib
    ${cocos-headers_SOURCE_DIR}/extensions/libExtensions.lib
    ${fmod_SOURCE_DIR}/lib/x86/fmod_vc.lib
)

# Warnings as errors
set_property(TARGET GD-Roulette PROPERTY COMPILE_WARNING_AS_ERROR ON)

add_compile_definitions(
    "_CRT_SECURE_NO_WARNINGS"
    "NOMINMAX"
    "WIN32_LEAN_AND_MEAN"
)

# Enable dev console if building in RelWithDebInfo
target_compile_definitions(GD-Roulette PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:DEV_CONSOLE>")
