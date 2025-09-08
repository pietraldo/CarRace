add_executable (rendering_3d_scene main.cpp ../externals/stb_image/stb_image.h Mesh.h Mesh.cpp Model.h Structs.h Shader.h Model.cpp Cube.cpp Cube.h Light.cpp Light.h LightDirectional.cpp LightDirectional.h LightPoint.h LightPoint.cpp LightSpot.cpp LightSpot.h Scene.h Scene.cpp Constants.h Camera.h Sphere.h Sphere.cpp)

set_source_files_properties(vertex_shader.txt fragment_shader.txt PROPERTIES HEADER_FILE_ONLY TRUE)
add_custom_target(extra_files ALL SOURCES vertex_shader.txt fragment_shader.txt vertex_shader2.txt fragment_shader2.txt fragment_spider.txt vertex_spider.txt)

target_include_directories(rendering_3d_scene PRIVATE ${CMAKE_SOURCE_DIR}/rendering_3d_scene)
target_include_directories(rendering_3d_scene PRIVATE ${CMAKE_SOURCE_DIR}/externals/imgui/imgui/backends)
target_link_libraries(rendering_3d_scene glfw glad imgui glm assimp)


