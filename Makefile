all: build run

build:
	g++ \
		-std=c++17 \
		-fdiagnostics-color=always \
		-framework OpenGL \
		-framework Cocoa \
		-framework IOKit \
		-framework CoreVideo \
		-framework CoreFoundation \
		-W -Wno-deprecated -Wall \
		-g \
		-o bin/a.out \
		-Iinclude \
		-Llib \
		-rpath lib \
		lib/libassimp.dylib \
		lib/libglfw.3.dylib \
		include/glad/glad.c \
		src/main.cpp

run:
	bin/a.out
