cmake_minimum_required(VERSION 3.14)
project(click-game)

include(FetchContent)

# SDL
FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-2.24.1)
FetchContent_MakeAvailable(SDL)

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
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl*)
add_library(imgui STATIC ${imgui_sources} ${imgui_backends})
target_compile_features(imgui PRIVATE cxx_std_17)
target_include_directories(imgui PRIVATE ${imgui_SOURCE_DIR})
target_include_directories(imgui PRIVATE ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui PRIVATE SDL2::SDL2-static)

file(GLOB_RECURSE SOURCES
	${PROJECT_SOURCE_DIR}/src/*.h
	${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
target_precompile_headers(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src/pch.h)
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /utf-8>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-W -Wall>)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:DEBUG>>:_CRTDBG_MAP_ALLOC>)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${imgui_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${imgui_SOURCE_DIR}/backends)
target_link_libraries(${PROJECT_NAME} PRIVATE
    imgui
    SDL2::SDL2-static SDL2::SDL2main)