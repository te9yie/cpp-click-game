cmake_minimum_required(VERSION 3.14)
project(click)

cmake_policy(SET CMP0135 NEW)

find_package(OpenGL REQUIRED)

include(FetchContent)

# SDL
FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-2.24.1)
FetchContent_MakeAvailable(SDL)

# glew
FetchContent_Declare(
    glew
    URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip
    SOURCE_SUBDIR build/cmake
)
set(BUILD_UTILS OFF CACHE BOOL "")
FetchContent_MakeAvailable(glew)

# imgui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.88)
FetchContent_MakeAvailable(imgui)

file(GLOB imgui_sources
    ${imgui_SOURCE_DIR}/*.h
    ${imgui_SOURCE_DIR}/*.cpp)
file(GLOB imgui_backends
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3*
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl.*)
source_group(imgui FILES ${imgui_sources} ${imgui_backends})

file(GLOB_RECURSE sources
	${PROJECT_SOURCE_DIR}/src/*.h
	${PROJECT_SOURCE_DIR}/src/*.cpp)

source_group(imgui FILES ${imgui_sources} ${imgui_backends})

add_executable(${PROJECT_NAME} WIN32
    ${imgui_sources} ${imgui_backends}
    ${sources})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /utf-8>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-W -Wall>)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:DEBUG>>:_CRTDBG_MAP_ALLOC>)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${imgui_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${imgui_SOURCE_DIR}/backends)
target_include_directories(${PROJECT_NAME} PRIVATE ${glew_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2::SDL2-static SDL2::SDL2main
    glew_s OpenGL::GL OpenGL::GLU)