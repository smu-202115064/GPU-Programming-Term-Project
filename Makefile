all: build run

build:
	g++ \
		-std=c++17 \
		-fdiagnostics-color=always \
		-Wall \
		-g \
		-Iinclude \
		-Llib \
		-rpath \
		lib \
		include/glad/glad.c \
		src/main.cpp \
		lib/libassimp.dylib \
		lib/libglfw.3.dylib \
		-o \
		bin/a.out \
		-framework \
		OpenGL \
		-framework \
		Cocoa \
		-framework \
		IOKit \
		-framework \
		CoreVideo \
		-framework \
		CoreFoundation \
		-W \
		-Wno-deprecated

run:
	bin/a.out
