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
)

target_link_libraries(carrace PRIVATE glfw glad imgui glm assimp)

# (opcjonalnie) Dołącz nagłówki, żeby były widoczne w IDE
file(GLOB_RECURSE SRC_HEADERS CONFIGURE_DEPENDS
  app/*.h core/*.h gfx/*.h game/*.h features/*.h physics/*.h ui/*.h
)
target_sources(carrace PRIVATE ${SRC_HEADERS})

# (opcjonalnie) shadery jako "pliki dodatkowe"
#set(SHADER_FILES
  #${CMAKE_SOURCE_DIR}/assets/shaders/vertex_shader.txt
  #${CMAKE_SOURCE_DIR}/assets/shaders/fragment_shader.txt
#)
target_sources(carrace PRIVATE ${SHADER_FILES})

# Grupowanie w Solution Explorer
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SRC_FILES} ${SRC_HEADERS} ${SHADER_FILES})

if (MSVC)
  target_compile_options(carrace PRIVATE /W4 /permissive-)
else()
  target_compile_options(carrace PRIVATE -Wall -Wextra -Wpedantic)
endif()