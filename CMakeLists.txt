cmake_minimum_required(VERSION 3.5)

project(PatternGen LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(PatternGen main.c)

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(PatternGen glfw)

find_package(GLEW REQUIRED)
target_link_libraries(PatternGen GLEW::glew)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
target_link_libraries(PatternGen OpenGL::GL)
