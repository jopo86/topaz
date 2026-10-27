# ---------------------------------------------------------------------------
# CMakePresets
# ---------------------------------------------------------------------------
# cmake --preset release          | configure
# cmake --build --preset release  | build
# ctest --preset release          | test
# cmake --workflow --preset test  | configure, build and test in one command
#
# Debug + sanitizers (ASan, plus UBSan on GCC/Clang), built in build-debug/:
# cmake --preset debug-asan             | configure
# cmake --build --preset debug-asan     | build
# ctest --preset debug-asan             | test
# cmake --workflow --preset test-asan   | configure, build and test in one command
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
option(TPZ_ENABLE_SANITIZERS  "Enable ASan (+UBSan on GCC/Clang), Debug" OFF)
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

  # MSVC only has ASan (no UBSan). It is incompatible with /RTC runtime
  # checks, which CMake adds to Debug flags by default, and with incremental
  # linking. Third-party code is built without ASan, so the STL container
  # annotations (string, vector, optional, ...) are all disabled to avoid
  # LNK2038 annotate_* mismatches.
  if(TPZ_ENABLE_SANITIZERS)
    string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    target_compile_options(tpz_options INTERFACE
      $<$<CONFIG:Debug>:/fsanitize=address>)
    target_compile_definitions(tpz_options INTERFACE
      $<$<CONFIG:Debug>:_DISABLE_STL_ANNOTATION>)
    target_link_options(tpz_options INTERFACE
      $<$<CONFIG:Debug>:/INCREMENTAL:NO>)
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

# MSVC ASan executables need the ASan runtime DLL, which lives next to cl.exe
# and is only on PATH in a Developer shell. Copy it next to the executable so
# it runs from any shell (including gtest_discover_tests' post-build run).
# Call this before gtest_discover_tests so the copy happens first.
function(tpz_copy_asan_runtime target)
  if(NOT (MSVC AND TPZ_ENABLE_SANITIZERS))
    return()
  endif()
  cmake_path(GET CMAKE_CXX_COMPILER PARENT_PATH compiler_dir)
  foreach(dll IN ITEMS clang_rt.asan_dynamic-x86_64.dll clang_rt.asan_dbg_dynamic-x86_64.dll)
    if(EXISTS "${compiler_dir}/${dll}")
      add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${compiler_dir}/${dll}" "$<TARGET_FILE_DIR:${target}>"
        VERBATIM)
    endif()
  endforeach()
endfunction()

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
      include/topaz/dsa/list.hpp
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