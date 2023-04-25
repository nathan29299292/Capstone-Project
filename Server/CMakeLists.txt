cmake_minimum_required(VERSION 3.5)

project(PatternGen LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_BUILD_TYPE MinSizeRel)
set(CMAKE_BUILD_TYPE Debug)

add_executable(PatternGen main.c gui.h gui.c shader.h shader.c canvas.h canvas.c)

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

file(COPY fragment.glsl vertex.glsl Source_Sans_Pro DESTINATION ${CMAKE_BINARY_DIR})
