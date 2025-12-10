# Google Test Configuration
add_subdirectory(${CMAKE_SOURCE_DIR}/externals/googletest ${CMAKE_BINARY_DIR}/googletest)

# Set PhysX paths
set(PHYSX_LIB ${CMAKE_SOURCE_DIR}/externals/physx/lib)

# Collect all test source files (excluding test_utils which is header-only)
file(GLOB_RECURSE TEST_SOURCES
    game/*.cpp
    gfx/*.cpp
    ui/*.cpp
    physics/*.cpp
    audio/*.cpp
)

# Create test executable
add_executable(carrace_tests ${TEST_SOURCES})

# Include directories for tests
target_include_directories(carrace_tests PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/externals
    ${CMAKE_SOURCE_DIR}/externals/glm
    ${CMAKE_SOURCE_DIR}/externals/physx/include
    ${CMAKE_SOURCE_DIR}/externals/rapidjson/include
    ${CMAKE_SOURCE_DIR}/externals/glad/include
    ${CMAKE_SOURCE_DIR}/externals/stb_image
    ${CMAKE_SOURCE_DIR}/externals/glfw/include
    ${CMAKE_SOURCE_DIR}/externals/imgui/imgui
    ${CMAKE_SOURCE_DIR}/externals/mini_audio
    ${CMAKE_SOURCE_DIR}/externals/hidapi/include
    ${CMAKE_SOURCE_DIR}/externals/assimp/include
    ${CMAKE_BINARY_DIR}/externals/assimp/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${gtest_SOURCE_DIR}/include
)

# Link Google Test and necessary libraries
target_link_libraries(carrace_tests PRIVATE
    gtest
    gtest_main
    glm
    glad
    ${PHYSX_LIB}/PhysXFoundation_64.lib
    ${PHYSX_LIB}/PhysXCommon_64.lib
)

# Ensure glad is built before tests
add_dependencies(carrace_tests glad)


# We need to compile the source files we're testing
# Add helper_functions.cpp and other testable source files
target_sources(carrace_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/game/helper_functions.cpp
    ${CMAKE_SOURCE_DIR}/externals/glad/src/glad.c
)

# Enable testing
enable_testing()
add_test(NAME carrace_tests COMMAND carrace_tests)

# Group test files in IDE
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TEST_SOURCES})

