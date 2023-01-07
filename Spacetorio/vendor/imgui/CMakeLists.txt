cmake_minimum_required(VERSION 3.8)
set (CMAKE_CXX_STANDARD 20)

project("ImGui")

# Add the ImGui source code.
add_library(imgui
  "imgui.cpp"
  "imgui_draw.cpp"
  "imgui_demo.cpp"
  "imgui_tables.cpp"
  "imgui_widgets.cpp"
  "backends/imgui_impl_sdl.cpp"
  "backends/imgui_impl_sdlrenderer.cpp"
)

# Add the include directory to the include path.
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


#target_compile_definitions(imgui PUBLIC -DIMGUI_IMPL_SDL)

# Install the ImGui library.
install(TARGETS imgui DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include
  FILES_MATCHING PATTERN "*.h")
