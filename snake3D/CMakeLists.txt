cmake_minimum_required(VERSION 3.10)
project(snake3D)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    ${CMAKE_SOURCE_DIR}/glfw-3.4/include
    ${CMAKE_SOURCE_DIR}/Glad/include
    ${CMAKE_SOURCE_DIR}/glm
)

add_executable(snake3D 
    snake3D.cpp
    glad.c
    Shader.cpp
)

set(CMAKE_EXE_LINKER_FLAGS "-static")

target_link_directories(snake3D PRIVATE
    ${CMAKE_SOURCE_DIR}/glfw-3.4/build/src
)
target_link_libraries(snake3D
    glfw3
)