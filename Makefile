# -*- MakeFile -*-

all: run

run: build/emulator.exe
	cd build && wine emulator.exe

build/emulator.exe: src/emulator.cpp
	x86_64-w64-mingw32-g++ src/emulator.cpp -o build/emulator.exe -lws2_32
