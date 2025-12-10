cmake_minimum_required(VERSION 3.25)
project(carrace LANGUAGES CXX C)

add_subdirectory(externals/glfw)
add_subdirectory(externals/glad)
add_subdirectory(externals/imgui)
add_subdirectory(externals/glm)
add_subdirectory(externals/assimp)



add_subdirectory(src)

# Add tests subdirectory
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

#set startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT carrace)