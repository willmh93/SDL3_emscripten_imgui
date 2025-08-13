cmake_minimum_required(VERSION 3.16)
project(bitloop VERSION 1.0.0 LANGUAGES C CXX)

add_executable(my_app main.cpp)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)

target_sources(my_app PRIVATE ${IMGUI_SOURCES})
target_include_directories(my_app PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_compile_definitions(my_app PRIVATE IMGUI_IMPL_OPENGL_ES3=1)

target_link_options(my_app PRIVATE
    "-sUSE_SDL=3"
    "-sUSE_WEBGL2=1"
    "-sFULL_ES3=1"
    "--shell-file=${CMAKE_SOURCE_DIR}/shell.html")

set(CMAKE_EXECUTABLE_SUFFIX ".html")