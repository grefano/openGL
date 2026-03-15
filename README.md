
# Dependências
ImGui, 
ffmpeg (libavformat, libavcodec, libavutil, libswscale)
glfw

# Compilar
use o comando
```cmake -B build -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++```
usar GCC é necessário para linkar o *imgui* e *glad* (arquivos .o)
