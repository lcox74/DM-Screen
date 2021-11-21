@set INCLUDES=-Ivendor/glad/include -Ivendor/glfw/include -Ivendor/imgui/include -Ivendor/stb
@set SRCS=src/main.cpp vendor/glad/src/gl.c vendor/imgui/src/imgui*.cpp
@set LIBDIRS=-Ivendor/glad/include -Ivendor/glfw/include -Ivendor/imgui -Lvendor/glfw/lib-mingw
@set LIBS=-lmingw32 -lglfw3 -lopengl32

mkdir bin
copy vendor\glfw\lib-mingw\glfw3.dll bin
g++ %INCLUDES% %SRCS% %LIBDIRS% %LIBS% -o bin/DMScreen.exe

