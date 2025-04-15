cmake_minimum_required(VERSION 3.5.0)
project(planet_sim VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)


file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

add_executable(planet_sim ${SOURCES})

target_include_directories(planet_sim PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(planet_sim
    PRIVATE
    glfw
    glm
)

