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

# # SDL config
set(SDL_STATIC ON)
set(SDL_SHARED OFF)
set(SDL_TEST_LIBRARIES OFF)
set(SDL_TESTS ${SDL_TEST_LIBRARIES})
set(SDL_TEST_LIBRARY ${SDL_TEST_LIBRARIES})

# # Provide dependencies
include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        062caeaa4b7ab05ed852121271b35205a9b9721c
    FIND_PACKAGE_ARGS
)

FetchContent_Declare(
    GLEW
    URL https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0-win32.zip/download
    URL_HASH SHA256=80cfc88fd295426b49001a9dc521da793f8547ac10aebfc8bdc91ddc06c5566c
)

FetchContent_Declare(
    IMGUI
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.0
)

FetchContent_MakeAvailable(SDL3 GLEW IMGUI)

find_package(OpenGL REQUIRED)

# # GLEW config
set(GLEW_INCLUDE_DIR ${glew_SOURCE_DIR}/include)
set(GLEW_LIB_DIR ${glew_SOURCE_DIR}/lib/Release/x64)
set(GLEW_BIN_DIR ${glew_SOURCE_DIR}/bin/Release/x64)

add_library(GLEW::GLEW-static STATIC IMPORTED)
set_target_properties(GLEW::GLEW-static PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${GLEW_INCLUDE_DIR}
    IMPORTED_LOCATION ${GLEW_LIB_DIR}/glew32s.lib
)

add_library(GLEW::GLEW SHARED IMPORTED)
set_target_properties(GLEW::GLEW PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${GLEW_INCLUDE_DIR}
    IMPORTED_LOCATION ${GLEW_BIN_DIR}/glew32.dll
    IMPORTED_IMPLIB ${GLEW_LIB_DIR}/glew32.lib
)

add_library(IMGUI STATIC
   ${imgui_SOURCE_DIR}/imgui.cpp
   ${imgui_SOURCE_DIR}/imgui_demo.cpp
   ${imgui_SOURCE_DIR}/imgui_draw.cpp
   ${imgui_SOURCE_DIR}/imgui_tables.cpp
   ${imgui_SOURCE_DIR}/imgui_widgets.cpp
   ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp 
   ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp 
)

target_include_directories(IMGUI PUBLIC
    ${imgui_SOURCE_DIR} 
    ${imgui_SOURCE_DIR}/backends
)

# Linking
target_link_libraries(IMGUI PRIVATE SDL3::SDL3-static)
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3-static
    GLEW::GLEW-static
    OPENGL32
    IMGUI
)