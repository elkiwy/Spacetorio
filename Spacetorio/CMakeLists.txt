# CMakeList.txt : CMake project for Spacetorio, include source and define
# project specific logic here.
#

if (APPLE)
  # Set the path to the SDL2 installation
  set(SDL2_PATH "/opt/homebrew/Cellar/sdl2/2.24.2")
  find_package(SDL2 REQUIRED)
elseif (WIN32)
  # Set the path to the SDL2 installation
  set(SDL2_PATH "C:/Program Files/SDL2/SDL2-2.26.1")
  set(SDL2_INCLUDE_DIRS "${SDL2_PATH}/include")
  set(SDL2_LIBRARIES "${SDL2_PATH}/lib/x64/SDL2.lib;${SDL2_PATH}/lib/x64/SDL2main.lib")
  set(SDL2_DLL "${SDL2_PATH}/lib/x64/SDL2.dll")

  set(SDL2_FONT_PATH "C:/Program Files/SDL2/SDL2_ttf-2.20.1")
  set(SDL2_FONT_INCLUDE_DIRS "${SDL2_FONT_PATH}/include")
  set(SDL2_FONT_LIBRARIES "${SDL2_FONT_PATH}/lib/x64/SDL2_ttf.lib")
  set(SDL2_FONT_DLL "${SDL2_FONT_PATH}/lib/x64/SDL2_ttf.dll")

endif()

# Cross platform paths
set(ENTT_INCLUDE_DIRS "vendor/entt/include")
set(IMGUI_INCLUDE_DIRS "vendor/imgui")
set(RES_FOLDER "res")

# Add the include directories for SDL2
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_FONT_INCLUDE_DIRS})
include_directories(${ENTT_INCLUDE_DIRS})
include_directories(${IMGUI_INCLUDE_DIRS})

# Add ImGui
add_subdirectory ("vendor/imgui")

# Add source to this project's executable.
file(GLOB_RECURSE SOURCE_FILES "./*.cpp" "./*.h")
add_executable(Spacetorio  ${SOURCE_FILES})

# Link against the SDL2 libraries
target_link_libraries(Spacetorio PRIVATE ${SDL2_LIBRARIES})
target_link_libraries(Spacetorio PRIVATE ${SDL2_FONT_LIBRARIES})
if (WIN32)
  # Copy DLL for SDL2
  add_custom_target(CopySDL2 ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL2_DLL}" "${CMAKE_BINARY_DIR}/Spacetorio")
  add_dependencies(Spacetorio CopySDL2)
  # Copy DLL for SDL2_FONT
  add_custom_target(CopySDL2_FONT ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL2_FONT_DLL}" "${CMAKE_BINARY_DIR}/Spacetorio")
  add_dependencies(Spacetorio CopySDL2_FONT)
endif()

# Add ImGui
target_link_libraries(Spacetorio PRIVATE imgui)


file(COPY "${RES_FOLDER}" DESTINATION "${CMAKE_BINARY_DIR}/Spacetorio")

# Set std=c++20
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Spacetorio PROPERTY CXX_STANDARD 20)
endif()

