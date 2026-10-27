# ---------------------------------------------------------------------------
# CMakePresets
# ---------------------------------------------------------------------------
# cmake --preset release          | configure
# cmake --build --preset release  | build
# ctest --preset release          | test
# cmake --workflow --preset test  | configure, build and test in one command

cmake_minimum_required(VERSION 3.25)

project(topaz
    VERSION 0.1.0
    DESCRIPTION "Low-latency limit order book and matching engine"
    LANGUAGES CXX
)

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(TPZ_BUILD_EXAMPLES     "Build the Dear ImGui example app"      ${PROJECT_IS_TOP_LEVEL})
option(TPZ_BUILD_TESTS        "Build unit tests (GoogleTest)"         ${PROJECT_IS_TOP_LEVEL})
option(TPZ_BUILD_BENCH        "Build benchmarks (Google Benchmark)"   ${PROJECT_IS_TOP_LEVEL})
option(TPZ_ENABLE_SANITIZERS  "Enable ASan/UBSan (GCC/Clang, Debug)"  OFF)
option(TPZ_WARNINGS_AS_ERRORS "Treat warnings as errors"              OFF)

# ---------------------------------------------------------------------------
# Global settings
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ---------------------------------------------------------------------------
# tpz_options: warnings + sanitizers for first-party targets only
# ---------------------------------------------------------------------------
add_library(tpz_options INTERFACE)

if(MSVC)
    target_compile_options(tpz_options INTERFACE /W4 /permissive- /Zc:__cplusplus)
    if(TPZ_WARNINGS_AS_ERRORS)
        target_compile_options(tpz_options INTERFACE /WX)
    endif()
else()
    target_compile_options(tpz_options INTERFACE
        -Wall -Wextra -Wpedantic -Wshadow -Wconversion)
    if(TPZ_WARNINGS_AS_ERRORS)
        target_compile_options(tpz_options INTERFACE -Werror)
    endif()

    if(TPZ_ENABLE_SANITIZERS)
        target_compile_options(tpz_options INTERFACE
            $<$<CONFIG:Debug>:-fsanitize=address,undefined -fno-omit-frame-pointer>)
        target_link_options(tpz_options INTERFACE
            $<$<CONFIG:Debug>:-fsanitize=address,undefined>)
    endif()
endif()

# ---------------------------------------------------------------------------
# Library: topaz (tpz::eng, tpz::dsa, tpz::net)
# Sources and headers are listed explicitly (no globbing).
# ---------------------------------------------------------------------------
add_library(topaz)
add_library(topaz::topaz ALIAS topaz)

target_sources(topaz
    PRIVATE
        src/engine/placeholder.cpp
        src/dsa/placeholder.cpp
        src/net/placeholder.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/topaz/engine/placeholder.hpp
            include/topaz/dsa/placeholder.hpp
            include/topaz/net/placeholder.hpp
)

target_include_directories(topaz
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(topaz PRIVATE tpz_options)

# Platform networking libs (adjust to match Garnet-derived code)
if(WIN32)
    target_link_libraries(topaz PUBLIC ws2_32)
endif()

# ---------------------------------------------------------------------------
# Subprojects (skipped automatically until their CMakeLists.txt exists)
# ---------------------------------------------------------------------------
if(TPZ_BUILD_TESTS AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
    enable_testing()
    add_subdirectory(tests)
endif()

if(TPZ_BUILD_BENCH AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bench/CMakeLists.txt)
    add_subdirectory(bench)
endif()

if(TPZ_BUILD_EXAMPLES AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/examples/gui/CMakeLists.txt)
    add_subdirectory(examples/gui)
endif()