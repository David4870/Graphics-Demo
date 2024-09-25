cmake_minimum_required(VERSION 3.30.2)
project(Graphics-Demo)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCHITECTURE x64)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(ARCHITECTURE x32)
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin/${ARCHITECTURE}/${CMAKE_BUILD_TYPE})

file(GLOB_RECURSE PRJ_SOURCE CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${PRJ_SOURCE})

set(BUILD_SHARED_LIBS OFF)

if(BUILD_SHARED_LIBS)
    set(MY_LIBRARIES SDL3::SDL3-shared glew OpenGL::GL)
    set(SDL_STATIC OFF)
    set(SDL_SHARED ON)
else()
    set(MY_LIBRARIES SDL3::SDL3-static glew_s OpenGL::GL)
    set(SDL_STATIC ON)
    set(SDL_SHARED OFF)
endif()
set(SDL_TEST_LIBRARIES OFF)
set(SDL_TESTS ${SDL_TEST_LIBRARIES})
set(SDL_TEST_LIBRARY ${SDL_TEST_LIBRARIES})

# OpenGL config
if(UNIX)
    cmake_policy(SET CMP0072 NEW)
    set(OpenGL_GL_PREFERENCE LEGACY)
endif()

# Provide dependencies
include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        062caeaa4b7ab05ed852121271b35205a9b9721c
)

FetchContent_Declare(
    GLEW
    URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip
    URL_HASH SHA256=A9046A913774395A095EDCC0B0AC2D81C3AACCA61787B39839B941E9BE14E0D4
)

FetchContent_Declare(
    IMGUI
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.0
)

FetchContent_MakeAvailable(SDL3 GLEW IMGUI)

add_subdirectory(${glew_SOURCE_DIR}/build/cmake)

find_package(OpenGL REQUIRED)

target_sources(${PROJECT_NAME} PRIVATE
   ${imgui_SOURCE_DIR}/imgui.cpp
   ${imgui_SOURCE_DIR}/imgui_demo.cpp
   ${imgui_SOURCE_DIR}/imgui_draw.cpp
   ${imgui_SOURCE_DIR}/imgui_tables.cpp
   ${imgui_SOURCE_DIR}/imgui_widgets.cpp
   ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp 
   ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp 
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${imgui_SOURCE_DIR} 
    ${imgui_SOURCE_DIR}/backends
)

target_include_directories(${PROJECT_NAME} PRIVATE ${glew_SOURCE_DIR}/include)

# Linking
target_link_libraries(${PROJECT_NAME} PRIVATE ${MY_LIBRARIES})