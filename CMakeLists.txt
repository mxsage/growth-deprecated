cmake_minimum_required(VERSION 3.8.2)
project(simulation)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(include external/tinyxml2)
include_directories(/usr/local/include/eigen3)
include_directories(/Users/sagejenson/dev/libigl/include)
include_directories(/Users/sagejenson/dev/cellgrow_test/external)

SET(OPENGL_APPLE "-lglfw -framework OpenGL -framework GLUT")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPENGL_APPLE}")

file(GLOB SOURCES "src/*.cpp")
list(APPEND SOURCES external/tinyxml2/tinyxml2.cpp)

add_executable(simulation ${SOURCES})
