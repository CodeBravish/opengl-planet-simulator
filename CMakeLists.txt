cmake_minimum_required(VERSION 3.5.0)
project(glfw_opengl VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)


set(SOURCES
    src/main.cpp
    src/glad.c
)

add_executable(glfw_opengl ${SOURCES})

target_link_libraries(glfw_opengl
    PRIVATE
    glfw
    glm
)

target_include_directories(glfw_opengl PRIVATE ${PROJECT_SOURCE_DIR}/include)
