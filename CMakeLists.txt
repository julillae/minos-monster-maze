cmake_minimum_required(VERSION 3.1)
project(436d)

# Set c++11
# https://stackoverflow.com/questions/10851247/how-to-activate-c-11-in-cmake
if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif ()
set (CMAKE_CXX_STANDARD 11)

# nice hierarchichal structure in MSVC
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#Find OS
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(IS_OS_MAC 1)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(IS_OS_LINUX 1)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(IS_OS_WINDOWS 1)
else()
  message(FATAL_ERROR "OS ${CMAKE_SYSTEM_NAME} was not recognized")
endif()

# Create executable target

# Generate the shader folder location to the header
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/project_path.hpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/include/project_path.hpp")

# You can switch to use the file GLOB for simplicity but at your own risk
# file(GLOB SOURCE_FILES src/*.cpp src/*.hpp)

set(SOURCE_FILES
	src/a1.cpp
	src/common.cpp
    src/renderManager.cpp
    src/spriteSheet.cpp
	src/renderEffects.cpp
    src/characters/character.cpp
    src/characters/player.cpp
    src/characters/enemy.cpp
    src/characters/smart.cpp
    src/characters/harpy.cpp
    src/characters/simple.cpp
    src/characters/spider.cpp
    src/mazeComponents/mazeComponent.cpp
    src/mazeComponents/fixedComponent.cpp
    src/mazeComponents/freeComponent.cpp
    src/mazeComponents/geometricComponent.cpp
    src/mazeComponents/floor.cpp
    src/mazeComponents/ice.cpp
    src/mazeComponents/exit.cpp
    src/mazeComponents/spike.cpp
    src/gameAI.cpp
    src/physics.cpp
    src/level.cpp
    src/stateTree.cpp
    src/menus/helpMenu.cpp
    src/game.cpp
    src/gameStates/gameState.cpp
    src/gameStates/mainMenuState.cpp
    src/gameStates/levelSelectState.cpp
    src/gameStates/pauseMenuState.cpp
    src/menus/mainMenu.cpp
    src/menus/menu.cpp
    src/menus/button.cpp
    src/menus/levelSelectMenu.cpp
    src/menus/levelButton.cpp
    src/menus/mainButton.cpp
    src/menus/pauseMenu.cpp
    src/menus/savePopup.cpp
    src/levelLoader.cpp
    src/quadTree.cpp
    src/gameSave.cpp


    include/project_path.hpp
	include/common.hpp
    include/renderManager.hpp
    include/spriteSheet.hpp
	include/renderEffects.hpp
    include/characters/character.hpp
    include/characters/player.hpp
    include/characters/enemy.hpp
    include/characters/smart.hpp
    include/characters/harpy.hpp
    include/characters/simple.hpp
    include/characters/spider.hpp
    include/mazeComponents/mazeComponent.hpp
    include/mazeComponents/fixedComponent.hpp
    include/mazeComponents/freeComponent.hpp
    include/mazeComponents/geometricComponent.hpp
    include/mazeComponents/floor.hpp
    include/mazeComponents/ice.hpp
    include/mazeComponents/exit.hpp
    include/mazeComponents/spike.hpp
    include/gameAI.hpp
    include/physics.hpp
    include/level.hpp
    include/stateTree.hpp
    include/menus/helpMenu.hpp
    include/game.hpp
    include/gameStates/gameState.hpp
    include/gameStates/mainMenuState.hpp
    include/gameStates/levelSelectState.hpp
    include/gameStates/pauseMenuState.hpp
    include/menus/mainMenu.hpp
    include/menus/menu.hpp
    include/menus/button.hpp
    include/menus/levelSelectMenu.hpp
    include/menus/levelButton.hpp
    include/menus/mainButton.hpp
    include/menus/pauseMenu.hpp
    include/menus/savePopup.hpp
    include/levelLoader.hpp
    include/quadTree.hpp
    include/gameSave.hpp

	)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC src/)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/levels
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Added this so policy CMP0065 doesn't scream
set_target_properties(${PROJECT_NAME} PROPERTIES ENABLE_EXPORTS 0)


# External header-only libraries in the ext/

target_include_directories(${PROJECT_NAME} PUBLIC ext/stb_image/)
target_include_directories(${PROJECT_NAME} PUBLIC ext/gl3w)
target_include_directories(${PROJECT_NAME} PUBLIC ext/rapidjson)

# Find OpenGL
find_package(OpenGL REQUIRED)

if (OPENGL_FOUND)
   target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIR})
   target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENGL_gl_LIBRARY})
endif()

# glfw, sdl could be precompiled (on windows) or installed by a package manager (on OSX and Linux)

if (IS_OS_LINUX OR IS_OS_MAC)
    # Try to find packages rather than to use the precompiled ones
    # Since we're on OSX or Linux, we can just use pkgconfig.
    find_package(PkgConfig REQUIRED)
    
    pkg_search_module(GLFW REQURIED glfw3)

    pkg_search_module(SDL2 REQURIED sdl2)
    pkg_search_module(SDL2MIXER REQURIED SDL2_mixer)
    
    # Link Frameworks on OSX
    if (IS_OS_MAC)
       find_library(COCOA_LIBRARY Cocoa)
       find_library(CF_LIBRARY CoreFoundation)
       target_link_libraries(${PROJECT_NAME} PUBLIC ${COCOA_LIBRARY} ${CF_LIBRARY})
    endif()

elseif (IS_OS_WINDOWS)
# https://stackoverflow.com/questions/17126860/cmake-link-precompiled-library-depending-on-os-and-architecture
    set(GLFW_FOUND TRUE)
    set(SDL2_FOUND TRUE)
    
    set(GLFW_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/include")
    set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/include/SDL")
    
    if (${CMAKE_SIZEOF_VOID_P} MATCHES "8")
        set(GLFW_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/lib/glfw3dll-x64.lib")
        set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2-x64.lib")
	set(SDL2MIXER_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2_mixer-x64.lib")

	set(GLFW_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/lib/glfw3-x64.dll")
	set(SDL_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2-x64.dll")
	set(SDLMIXER_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2_mixer-x64.dll")
    else()
        set(GLFW_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/lib/glfw3dll-x86.lib")
        set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2-x86.lib")
	set(SDL2MIXER_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2_mixer-x86.lib")
	
	set(GLFW_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/lib/glfw3-x86.dll")
	set(SDL_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2-x86.dll")
	set(SDLMIXER_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2_mixer-x86.dll")
    endif()

    # Copy and rename dlls
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GLFW_DLL}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/glfw3.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL_DLL}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL2.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDLMIXER_DLL}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL2_mixer.dll")
endif()

# Can't find the include and lib. Quit.
if (NOT GLFW_FOUND OR NOT SDL2_FOUND)
   if (NOT GLFW_FOUND)
      message(FATAL_ERROR "Can't find GLFW." )
   else ()
      message(FATAL_ERROR "Can't find SDL." )
   endif()
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC ${GLFW_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2MIXER_LIBRARIES})

# Needed to add this
if(IS_OS_LINUX)
  target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_DL_LIBS})
endif()
