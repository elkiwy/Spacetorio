# CMakeList.txt : CMake project for Spacetorio, include source and define
# project specific logic here.
#

# Set the path to the SDL2 installation


if (APPLE)
  set(SDL2_PATH "/opt/homebrew/Cellar/sdl2/2.24.2")
  find_package(SDL2 REQUIRED)
elseif (WIN32)
  set(SDL2_PATH "C:/Program Files/SDL2/SDL2-2.26.1")
  set(SDL2_INCLUDE_DIRS "${SDL2_PATH}/include")
  set(SDL2_LIBRARIES "${SDL2_PATH}/lib/x64/SDL2.lib;${SDL2_PATH}/lib/x64/SDL2main.lib")
  set(SDL2_DLL "${SDL2_PATH}/lib/x64/SDL2.dll")
endif()




# Add the include directories for SDL2
include_directories(${SDL2_INCLUDE_DIRS})

# Add source to this project's executable.
add_executable (Spacetorio "Spacetorio.cpp" "Spacetorio.h")

# Link against the SDL2 libraries
target_link_libraries(Spacetorio PRIVATE ${SDL2_LIBRARIES})


if (WIN32)
  add_custom_target(CopySDL2 ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL2_DLL}" "${CMAKE_BINARY_DIR}/Spacetorio")
  add_dependencies(Spacetorio CopySDL2)
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Spacetorio PROPERTY CXX_STANDARD 20)
endif()

