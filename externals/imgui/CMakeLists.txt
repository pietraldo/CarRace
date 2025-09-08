add_library(imgui STATIC imgui/backends/imgui_impl_glfw.cpp imgui/backends/imgui_impl_opengl3.cpp imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp)
target_include_directories(imgui PUBLIC ${CMAKE_SOURCE_DIR}/externals/imgui/imgui)
target_include_directories(imgui PUBLIC ${CMAKE_SOURCE_DIR}/externals/glfw/include)
