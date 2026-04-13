set(CMAKE_TOOLCHAIN_FILE "C:/Users/liorh/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_minimum_required(VERSION 3.20)
project(MyOpenGLProject)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/ShaderManager.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw
    glad::glad
    glm::glm
)

# copy shaders next to the exe after every build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/src/vertexShader.GLSL"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/vertexShader.GLSL"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/src/fragmentShader.GLSL"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/fragmentShader.GLSL"
)