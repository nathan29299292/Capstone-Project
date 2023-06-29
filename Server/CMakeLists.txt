cmake_minimum_required(VERSION 3.5)

project(PatternGen LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE MinSizeRel)

if (CMAKE_C_COMPILER_ID MATCHES "GNU")
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_executable(PatternGen main.c gui.h gui.c shader.h shader.c canvas.h canvas.c image.h image.c)

find_package(glfw3 REQUIRED)
include_directories(${GLFW3_INCLUDE_DIR})
target_link_libraries(PatternGen PUBLIC glfw)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(PatternGen PUBLIC OpenGL::GL)

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})
target_link_libraries(PatternGen PUBLIC GLEW)

target_link_libraries(PatternGen PUBLIC m)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/shaders/fragment.glsl ${CMAKE_CURRENT_BINARY_DIR}/shaders/fragment.glsl COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/shaders/vertex.glsl ${CMAKE_CURRENT_BINARY_DIR}/shaders/vertex.glsl COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/fonts/default.ttf ${CMAKE_CURRENT_BINARY_DIR}/fonts/default.ttf COPYONLY)
