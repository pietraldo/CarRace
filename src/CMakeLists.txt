file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  app/*.cpp
  core/*.cpp
  gfx/*.cpp
  game/*.cpp
  features/*.cpp
  physics/*.cpp
  ui/*.cpp
)

add_executable(carrace ${SRC_FILES})

target_include_directories(carrace PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}     # src/...
  ${CMAKE_SOURCE_DIR}/externals   # headery z externals (glad, stb, glm, imgui)
  ${CMAKE_SOURCE_DIR}             # opcjonalnie
  ${CMAKE_SOURCE_DIR}/externals/hidapi/include
  ${CMAKE_SOURCE_DIR}/externals/physx/include
)

include_directories(${CMAKE_SOURCE_DIR}/externals/physx/include)
set(PHYSX_DIR ${CMAKE_SOURCE_DIR}/externals/physx)
set(PHYSX_LIB ${CMAKE_SOURCE_DIR}/externals/physx/lib)

target_link_libraries(carrace PRIVATE
	glfw glad imgui glm assimp 
	${CMAKE_SOURCE_DIR}/externals/hidapi/lib/hidapi.lib
	
	
	${PHYSX_LIB}/PhysX_64.lib
	
	${PHYSX_LIB}/PhysXCommon_64.lib
	${PHYSX_LIB}/PhysXCooking_64.lib
	
	${PHYSX_LIB}/PhysXFoundation_64.lib
	
	
	
	${PHYSX_LIB}/PVDRuntime_64.lib
	
	${PHYSX_LIB}/PhysXExtensions_static_64.lib
	${PHYSX_LIB}/PhysXPvdSDK_static_64.lib
	${PHYSX_LIB}/SceneQuery_static_64.lib
	${PHYSX_LIB}/SimulationController_static_64.lib
	${PHYSX_LIB}/SnippetRender_static_64.lib
	${PHYSX_LIB}/SnippetUtils_static_64.lib
	${PHYSX_LIB}/PhysXVehicle2_static_64.lib
	${PHYSX_LIB}/PhysXTask_static_64.lib
	${PHYSX_LIB}/PhysXCharacterKinematic_static_64.lib
	${PHYSX_LIB}/LowLevel_static_64.lib
	${PHYSX_LIB}/LowLevelAABB_static_64.lib
	${PHYSX_LIB}/LowLevelDynamics_static_64.lib
	
)



# --- Add headers to project (IDE visibility) ---
file(GLOB_RECURSE SRC_HEADERS CONFIGURE_DEPENDS
  app/*.h core/*.h gfx/*.h game/*.h features/*.h physics/*.h ui/*.h
)
target_sources(carrace PRIVATE ${SRC_HEADERS})

# dodawanie shaderow
file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/assets/shaders/*.* 
)

target_sources(carrace PRIVATE ${SHADER_FILES})

# --- Function to copy a whole directory into build ---
function(copy_assets DIR_NAME)
  add_custom_target(copy_${DIR_NAME} ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_SOURCE_DIR}/assets/${DIR_NAME}
      ${CMAKE_BINARY_DIR}/assets/${DIR_NAME}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_SOURCE_DIR}/assets/${DIR_NAME}
      $<TARGET_FILE_DIR:carrace>/assets/${DIR_NAME}
    COMMENT "Copying ${DIR_NAME}..."
  )
  add_dependencies(carrace copy_${DIR_NAME})
endfunction()

# --- Copy all assets subdirectories (shaders, textures, models, etc.) ---
copy_assets(shaders)
copy_assets(models)

# --- Copy DLLs ---
add_custom_command(TARGET carrace POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/externals/hidapi/bin/hidapi.dll
    $<TARGET_FILE_DIR:carrace>
)


# --- Copy PhysX DLLs ---
set(PHYSX_DLL_DIR "${PHYSX_DIR}/dll") # adjust to your build output

file(GLOB PHYSX_DLLS "${PHYSX_DLL_DIR}/*.dll")

add_custom_target(copy_physx_dll ALL
  COMMENT "Copying PhysX DLLs..."
)

foreach(dll_file ${PHYSX_DLLS})
  add_custom_command(TARGET copy_physx_dll POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${dll_file}
      $<TARGET_FILE_DIR:carrace>
  )
endforeach()

add_dependencies(carrace copy_physx_dll)

# Grupowanie w Solution Explorer
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SRC_FILES} ${SRC_HEADERS} ${SHADER_FILES})

if (MSVC)
  target_compile_options(carrace PRIVATE /W4 /permissive-)
else()
  target_compile_options(carrace PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Ustaw projekt "carrace" jako domyślny startup project w Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT carrace)