cmake_minimum_required(VERSION 3.21.0)
project(aoc2022
  VERSION 0.1.0
  DESCRIPTION "Advent of Code 2022"
  HOMEPAGE_URL https://github.com/emanspeaks/aoc2022
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

## configure CMake module search paths that depend on the project
## proj src dir is location of this file
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(CompileCommands)
include(DefaultBuildType)
include(DefaultInstallPath)
include(ForceUnicode)
include(DefineWindows)
include(HideVSCodeVars)

add_subdirectory(day01)
add_subdirectory(day02)
add_subdirectory(day03)
add_subdirectory(day04)
add_subdirectory(day05)
add_subdirectory(day06)
add_subdirectory(day07)
add_subdirectory(day08)
# add_subdirectory(day09)
# add_subdirectory(day10)
# add_subdirectory(day11)
# add_subdirectory(day12)
# add_subdirectory(day13)
# add_subdirectory(day14)
add_subdirectory(day15)
add_subdirectory(day16)
