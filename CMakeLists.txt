cmake_minimum_required(VERSION 3.1)

project(raytracer)

set(CMAKE_BINARY_DIR ${CMAKE_BUILD_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories("${PROJECT_SOURCE_DIR}/include")
file(GLOB SRCS "src/*.cpp")

set(CMAKE_CXX_STANDARD 11)

add_executable(raytracer ${SRCS})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

find_package(ASSIMP REQUIRED)
include_directories(${ASSIMP_INCLUDE_DIR})

target_link_libraries(raytracer ${PNG_LIBRARY} assimp pthread)

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif (CMAKE_COMPILER_IS_GNUCC)

set(CMAKE_BUILD_TYPE RelWithDebInfo)
