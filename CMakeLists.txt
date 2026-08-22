cmake_minimum_required(VERSION 3.10)
project(LoggerProject)

# стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_STATS_APP "Build stats application" ON)

add_subdirectory(src/logger)
add_subdirectory(src/app)

if(BUILD_TESTS)
    add_subdirectory(src/tests)
endif()

if(BUILD_STATS_APP)
    add_subdirectory(src/stats_app)
endif()