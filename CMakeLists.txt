cmake_minimum_required(VERSION 3.25)
project(carrace LANGUAGES CXX C)

# Force Assimp to build as STATIC library to avoid DLL boundary issues with std::string/vectors
# when we are mixing Debug Runtime (/MDd) in a Release build configuration.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build package with shared libraries." FORCE)

# Force usage of Debug Runtime (/MDd) on MSVC for all configurations (Debug/Release)
# This is required because the PhysX static libraries in 'externals/physx/lib' are 
# compiled with /MDd (Debug), and we must match the runtime library to link successfully.
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")
    
    # Also adjust preprocessor definitions:
    # PhysX requires exactly ONE of NDEBUG or _DEBUG.
    # Since we force Debug runtime (/MDd), we MUST use _DEBUG and remove NDEBUG.
    foreach(config RELEASE MINSIZEREL RELWITHDEBINFO)
         string(TOUPPER ${config} config_upper)
         foreach(lang C CXX)
             set(flags_var "CMAKE_${lang}_FLAGS_${config_upper}")
             
             # Replace /MD (Release) with /MDd (Debug)
             string(REPLACE "/MD" "/MDd" ${flags_var} "${${flags_var}}")
             string(REPLACE "-MD" "-MDd" ${flags_var} "${${flags_var}}")

             # Replace /O2 (Maximize Speed) with /Od (Disable Optimization)
             # Mixing /O2 with /MDd and _ITERATOR_DEBUG_LEVEL=2 is risky and can cause crashes.
             string(REPLACE "/O2" "/Od" ${flags_var} "${${flags_var}}")
             string(REPLACE "-O2" "-Od" ${flags_var} "${${flags_var}}")
             
             # Also remove /Ob2 (Inline Expansion) just in case
             string(REPLACE "/Ob2" "" ${flags_var} "${${flags_var}}")


             string(REPLACE "/DNDEBUG" "" ${flags_var} "${${flags_var}}")
             string(REPLACE "-DNDEBUG" "" ${flags_var} "${${flags_var}}")
             # Append /D_DEBUG
             set(${flags_var} "${${flags_var}} /D_DEBUG")
         endforeach()
    endforeach()
endif()

# Disable warnings as errors for Assimp to prevent build failure on warnings
option(ASSIMP_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)

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