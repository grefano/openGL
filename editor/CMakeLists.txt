

cmake_minimum_required(VERSION 4.3.0)
project(myeditor CXX)

add_executable(editor)
target_sources(editor PRIVATE src/main.cpp libs/glad.o src/ffmpeg.cpp src/objects.cpp vendor/stb.cpp vendor/imgui/imgui.o vendor/imgui/imgui_demo.o vendor/imgui/imgui_draw.o vendor/imgui/imgui_impl_glfw.o vendor/imgui/imgui_impl_opengl3.o vendor/imgui/imgui_tables.o vendor/imgui/imgui_widgets.o)


set(MSYS2 "C:/msys64/ucrt64/")

target_include_directories(editor PRIVATE ${MSYS2}/include vendor/ vendor/imgui)

find_library(AVCODEC_LIB NAMES avcodec PATHS "${MSYS2}/lib" NO_DEFAULT_PATH)
find_library(AVFORMAT_LIB NAMES avformat PATHS "${MSYS2}/lib" NO_DEFAULT_PATH)
find_library(AVUTIL_LIB NAMES avutil PATHS "${MSYS2}/lib" NO_DEFAULT_PATH)
find_library(SWS_LIB NAMES swscale PATHS "${MSYS2}/lib" NO_DEFAULT_PATH)
find_library(GLFW_LIB NAMES glfw3 PATHS "${MSYS2}/lib" NO_DEFAULT_PATH)

target_link_libraries(editor PRIVATE ${AVCODEC_LIB} ${AVFORMAT_LIB} ${AVUTIL_LIB} ${SWS_LIB} ${GLFW_LIB})