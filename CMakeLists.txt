cmake_minimum_required(VERSION 3.1)
project(simulation)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(FRAMES_DIR "build/frames")
set(FRAMES_DIR "build/parameters")

# libigl
option(LIBIGL_USE_STATIC_LIBRARY "Use libigl as static library" ON)
option(LIBIGL_WITH_ANTTWEAKBAR      "Use AntTweakBar"    OFF)
option(LIBIGL_WITH_CGAL             "Use CGAL"           OFF)
option(LIBIGL_WITH_COMISO           "Use CoMiso"         OFF)
option(LIBIGL_WITH_CORK             "Use Cork"           OFF)
option(LIBIGL_WITH_EMBREE           "Use Embree"         OFF)
option(LIBIGL_WITH_LIM              "Use LIM"            OFF)
option(LIBIGL_WITH_MATLAB           "Use Matlab"         OFF)
option(LIBIGL_WITH_MOSEK            "Use MOSEK"          OFF)
option(LIBIGL_WITH_NANOGUI          "Use Nanogui menu"   ON)
option(LIBIGL_WITH_OPENGL           "Use OpenGL"         ON)
option(LIBIGL_WITH_OPENGL_GLFW      "Use GLFW"           ON)
option(LIBIGL_WITH_PNG              "Use PNG"            OFF)
option(LIBIGL_WITH_PYTHON           "Use Python"         OFF)
option(LIBIGL_WITH_TETGEN           "Use Tetgen"         OFF)
option(LIBIGL_WITH_TRIANGLE         "Use Triangle"       OFF)
option(LIBIGL_WITH_VIEWER           "Use OpenGL viewer"  ON)
option(LIBIGL_WITH_XML              "Use XML"            ON)

file(MAKE_DIRECTORY ${FRAMES_DIR})

find_package(LIBIGL REQUIRED QUIET)

file(GLOB SOURCES src/*.cpp)
add_executable(${PROJECT_NAME}_bin ${SOURCES})
target_link_libraries(${PROJECT_NAME}_bin igl::core igl::viewer igl::xml)
