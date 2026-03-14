

cmake_minimum_required(VERSION 4.3.0)
project(myeditor CXX)

add_executable(editor)
target_sources(editor PRIVATE src/main.cpp libs/glad.o src/ffmpeg.cpp src/opengl.cpp src/timeline.cpp src/shadercomponent.cpp vendor/stb.cpp vendor/imgui/docking/imgui.o vendor/imgui/docking/imgui_demo.o vendor/imgui/docking/imgui_draw.o vendor/imgui/docking/imgui_impl_glfw.o vendor/imgui/docking/imgui_impl_opengl3.o vendor/imgui/docking/imgui_tables.o vendor/imgui/docking/imgui_widgets.o)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED libavcodec libavformat libavutil libswscale)
find_package(glfw3 REQUIRED)

target_include_directories(editor PRIVATE ${FFMPEG_INCLUDE_DIRS} vendor/ vendor/imgui/docking)

target_link_libraries(editor PRIVATE ${FFMPEG_LIBRARIES} glfw3)