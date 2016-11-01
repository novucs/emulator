# -*- MakeFile -*-

all: run

run: build/emulator.exe
	cd build && wine emulator.exe

build/emulator.exe: src/emulator.c
	x86_64-w64-mingw32-g++ src/emulator.c -o build/emulator.exe -lws2_32
