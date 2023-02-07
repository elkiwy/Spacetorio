#
# CMakeList.txt : CMake project for Spacetorio, include source and define
# project specific logic here.
#

if (APPLE)
  # Set the path to the SDL2 installation
  find_package(SDL2 REQUIRED)
  set(SDL2_PATH "/opt/homebrew/Cellar/sdl2/2.24.2")
  set(SDL2_INCLUDE_DIRS "${SDL2_PATH}/include/SDL2")

  find_package(SDL2_TTF REQUIRED)
  set(SDL2_TTF_PATH "/opt/homebrew/Cellar/sdl2_ttf/2.20.1")
  set(SDL2_TTF_INCLUDE_DIRS "${SDL2_TTF_PATH}/include/SDL2")
  set(SDL2_TTF_LIBRARIES "${SDL2_TTF_PATH}/lib/libSDL2_ttf.a")

  SET(OPENGL_GLEW_PATH "/opt/homebrew/Cellar/glew/2.2.0_1")
  SET(OPENGL_GLEW_INCLUDE_DIR "${OPENGL_GLEW_PATH}/include")
  SET(OPENGL_GLEW_LIBRARIES "${OPENGL_GLEW_PATH}/lib/libGLEW.a")

  SET(CMAKE_EXE_LINKER_FLAGS "-v -lfreetype -lharfbuzz -framework OpenGL")

elseif (WIN32)
  # Set the path to the SDL2 installation
  set(SDL2_PATH "C:/Program Files/SDL2/SDL2-2.26.1")
  set(SDL2_INCLUDE_DIRS "${SDL2_PATH}/include")
  set(SDL2_LIBRARIES "${SDL2_PATH}/lib/x64/SDL2.lib;${SDL2_PATH}/lib/x64/SDL2main.lib")
  set(SDL2_DLL "${SDL2_PATH}/lib/x64/SDL2.dll")

  set(SDL2_TTF_PATH "C:/Program Files/SDL2/SDL2_ttf-2.20.1")
  set(SDL2_TTF_INCLUDE_DIRS "${SDL2_TTF_PATH}/include")
  set(SDL2_TTF_LIBRARIES "${SDL2_TTF_PATH}/lib/x64/SDL2_ttf.lib")
  set(SDL2_TTF_DLL "${SDL2_TTF_PATH}/lib/x64/SDL2_ttf.dll")

  set(SDL2_IMAGE_PATH "C:/Program Files/SDL2/SDL2_image-2.6.2")
  set(SDL2_IMAGE_INCLUDE_DIRS "${SDL2_IMAGE_PATH}/include")
  set(SDL2_IMAGE_LIBRARIES "${SDL2_IMAGE_PATH}/lib/x64/SDL2_image.lib")
  set(SDL2_IMAGE_DLL "${SDL2_IMAGE_PATH}/lib/x64/SDL2_image.dll")

  SET(OPENGL_GLEW_PATH "C:/Program Files/GLEW")
  SET(OPENGL_GLEW_INCLUDE_DIR "${OPENGL_GLEW_PATH}/include")
  SET(OPENGL_GLEW_LIBRARIES "${OPENGL_GLEW_PATH}/lib/Release/x64/glew32.lib")
  set(OPENGL_GLEW_DLL "${OPENGL_GLEW_PATH}/bin/Release/x64/glew32.dll")

endif()





# Cross platform paths
set(GLM_INCLUDE_DIRS "vendor/glm")
set(SIVPERLIN_INCLUDE_DIRS "vendor/SivPerlinNoise/include")
set(ENTT_INCLUDE_DIRS "vendor/entt/include")
set(IMGUI_INCLUDE_DIRS "vendor/imgui")
set(RES_FOLDER "res")
set(SHADER_FOLDER "shaders")
set(STB_INCLUDE_DIRS "vendor/stb")

# Add the include directories for SDL2
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_TTF_INCLUDE_DIRS})
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
include_directories(${OPENGL_GLEW_INCLUDE_DIR})

include_directories(${IMGUI_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})
include_directories(${STB_INCLUDE_DIRS})
include_directories(${SIVPERLIN_INCLUDE_DIRS})
include_directories(${ENTT_INCLUDE_DIRS})

# Add ImGui
add_subdirectory ("vendor/imgui")

# Add source to this project's executable.
file(GLOB_RECURSE SOURCE_FILES "./*.cpp" "./*.h")
add_executable(Spacetorio  ${SOURCE_FILES})

# Link against the SDL2 libraries
target_link_libraries(Spacetorio PRIVATE ${SDL2_LIBRARIES})
target_link_libraries(Spacetorio PRIVATE ${SDL2_TTF_LIBRARIES})
target_link_libraries(Spacetorio PRIVATE ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(Spacetorio PRIVATE ${OPENGL_GLEW_LIBRARIES})



if (WIN32)
  target_link_libraries(Spacetorio PRIVATE opengl32)

  # Copy DLL for SDL2
  add_custom_target(CopySDL2 ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL2_DLL}" "${CMAKE_BINARY_DIR}/Spacetorio")
  add_dependencies(Spacetorio CopySDL2)
  # Copy DLL for SDL2_TTF
  add_custom_target(CopySDL2_TTF ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL2_TTF_DLL}" "${CMAKE_BINARY_DIR}/Spacetorio")
  add_dependencies(Spacetorio CopySDL2_TTF)
  # Copy DLL for SDL2_IMAGE
  add_custom_target(CopySDL2_IMAGE ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL2_IMAGE_DLL}" "${CMAKE_BINARY_DIR}/Spacetorio")
  add_dependencies(Spacetorio CopySDL2_IMAGE)
  # Copy DLL for GLEW
  add_custom_target(CopyOPENGL_GLEW ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OPENGL_GLEW_DLL}" "${CMAKE_BINARY_DIR}/Spacetorio")
  add_dependencies(Spacetorio CopyOPENGL_GLEW)
endif()

# Add ImGui
target_link_libraries(Spacetorio PRIVATE imgui)


file(COPY "${RES_FOLDER}" DESTINATION "${CMAKE_BINARY_DIR}/Spacetorio")
file(COPY "${SHADER_FOLDER}" DESTINATION "${CMAKE_BINARY_DIR}/Spacetorio")

# Set std=c++20
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Spacetorio PROPERTY CXX_STANDARD 20)
endif()

