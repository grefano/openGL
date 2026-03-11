

cmake_minimum_required(VERSION 4.3.0)
project(myeditor CXX)

set(DIR_IMGUI vendor/imgui/docking/)
add_executable(editor)
target_sources(editor PRIVATE src/main.cpp libs/glad.o vendor/stb.cpp
src/application/render.cpp
src/infrastructure/ffmpeg.cpp src/infrastructure/opengl.cpp src/infrastructure/mediapool.cpp src/infrastructure/timelinewalker.cpp
src/domain/timeline.cpp src/domain/shadercomponent.cpp 
${DIR_IMGUI}imgui.o ${DIR_IMGUI}imgui_demo.o ${DIR_IMGUI}imgui_draw.o ${DIR_IMGUI}imgui_impl_glfw.o ${DIR_IMGUI}imgui_impl_opengl3.o ${DIR_IMGUI}imgui_tables.o ${DIR_IMGUI}imgui_widgets.o)


set(MSYS2 "C:/msys64/ucrt64/")
target_include_directories(editor PRIVATE src src/domain src/application src/infrastructure ${MSYS2}/include vendor/ ${DIR_IMGUI})

find_library(AVCODEC_LIB NAMES avcodec PATHS "${MSYS2}/lib" NO_DEFAULT_PATH)
find_library(AVFORMAT_LIB NAMES avformat PATHS "${MSYS2}/lib" NO_DEFAULT_PATH)
find_library(AVUTIL_LIB NAMES avutil PATHS "${MSYS2}/lib" NO_DEFAULT_PATH)
find_library(SWS_LIB NAMES swscale PATHS "${MSYS2}/lib" NO_DEFAULT_PATH)
find_library(GLFW_LIB NAMES glfw3 PATHS "${MSYS2}/lib" NO_DEFAULT_PATH)

target_link_libraries(editor PRIVATE ${AVCODEC_LIB} ${AVFORMAT_LIB} ${AVUTIL_LIB} ${SWS_LIB} ${GLFW_LIB})